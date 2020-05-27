#include <avr/io.h>
#include <avr/interrupt.h>
#include <Bounce2.h>
#include "AudioSystem.h"

// Pins
#define GAININ_PIN      A3 //was 0
#define FILTER_PIN      A2 //was 1
#define WET_PIN         A1 //was 2
#define VOL_PIN         A0 //was 3
#define FX1_PIN         4  //
#define FX2_PIN         5  
#define FX3_PIN         6  
#define FX4_PIN         9 
#define LED_FX1_PIN     11 //REVERB
#define LED_FX2_PIN     10 //DELAY
#define LED_FX3_PIN     2  //FLANGE
#define LED_FX4_PIN     3  //CHORUS
#define SYSTEM_LED      13

#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES)

// Constants
#define ON  1.0
#define OFF 0.0

enum StateFlags {
    S_REVERB = 0x1 << 0,
    S_DELAY  = 0x1 << 1,
    S_FLANGE = 0x1 << 2,
    S_CHORUS = 0x1 << 3,
};

const int kFilterFreqMax = 12000;
const float kGainInMax = 0.8;
const float kVolumeMax = 0.5; //was 0.5

elapsedMillis msec = 0;
const unsigned tUpdate = 1;
float vGainIn = kGainInMax;
float vVolume = kVolumeMax;
float vWet = 0.5;
int vFilterFreq = 10000;
int counter = 0;

short toggle = 0;

uint8_t state = 0x0;

short flangeDelayLineL[FLANGE_DELAY_LENGTH];
short flangeDelayLineR[FLANGE_DELAY_LENGTH];
short chorusDelayLineL[CHORUS_DELAY_LENGTH];
short chorusDelayLineR[CHORUS_DELAY_LENGTH];

Bounce bFx1Bypass = Bounce(FX1_PIN,15);
Bounce bFx2Bypass = Bounce(FX2_PIN,15);
Bounce bFx3Bypass = Bounce(FX3_PIN,15);
Bounce bFx4Bypass = Bounce(FX4_PIN,15);

void sayHello(){
    //running
    for(int i = 0; i < 3; i++){
      digitalWrite(LED_FX1_PIN, HIGH);
      delay(100);
      digitalWrite(LED_FX1_PIN, LOW);
      delay(100);
      
      digitalWrite(LED_FX2_PIN, HIGH);
      delay(100); 
      digitalWrite(LED_FX2_PIN, LOW);
      delay(100);
      
      digitalWrite(LED_FX3_PIN, HIGH);
      delay(100);  
      digitalWrite(LED_FX3_PIN, LOW);
      delay(100);
      
      digitalWrite(LED_FX4_PIN, HIGH);
      delay(100);  
      digitalWrite(LED_FX4_PIN, LOW);
      delay(100);
    }
    //flashing
    for(int i = 0; i < 3; i++){
      digitalWrite(LED_FX1_PIN, HIGH);
      digitalWrite(LED_FX2_PIN, HIGH);
      digitalWrite(LED_FX3_PIN, HIGH);
      digitalWrite(LED_FX4_PIN, HIGH);
      delay(200);  
      
      digitalWrite(LED_FX1_PIN, LOW);
      digitalWrite(LED_FX2_PIN, LOW);
      digitalWrite(LED_FX3_PIN, LOW);
      digitalWrite(LED_FX4_PIN, LOW);
      delay(200);
    }
    
    digitalWrite(SYSTEM_LED, HIGH);
    toggle = HIGH;
  }

void setup() {
    Serial.begin(9600);

    AudioMemory(500);   // Allocate memory for Audio connections

    configurePins();
    configureAudioAdaptor();

    setGainIn(vGainIn);

    initializeState();

    configureMixerMaster();
    configureMixerFx();
    configureReverb();
    configureDelay();
    configureFlange();
    configureChorus();

    switchLogic();

    sayHello();
}


void loop()
{
    if ((msec > tUpdate))
    {
        vGainIn = (float) analogRead(A3) / (1023.0 / kGainInMax); //GAININ_PIN
        vFilterFreq = (float) analogRead(A2) / 1023.0 * kFilterFreqMax;//FILTER_PIN
        vWet = (float) analogRead(A1) / (1023.0); //WET_PIN
        vVolume = (float) analogRead(A0) / (1023.0 / kVolumeMax); //VOL_PIN

        setGainIn(vGainIn);
        setFilterFreq(vFilterFreq);

        updateState();

        setDryWetBalance();

        sgtl5000.volume(vVolume);

        if(counter > 999){          
          printParameters();
          counter = 0;
        }
        counter++;
        msec = 0;
    }     
//    if(toggle){
//      digitalWrite(SYSTEM_LED, toggle);
//      toggle = LOW;
//    }
//    else{
//      digitalWrite(SYSTEM_LED, toggle);
//      toggle = HIGH;
//    }
//    //delay(50);
}


void updateState()
{
    bFx4Bypass.update();
    bFx3Bypass.update();
    bFx2Bypass.update();
    bFx1Bypass.update();

    if (bFx4Bypass.fallingEdge())
    {
        state ^= S_REVERB;

        if (state & S_REVERB)
        {
            digitalWrite(LED_FX1_PIN, HIGH);
            Serial.println("REVERB ON");
        }
        else
        {
            digitalWrite(LED_FX1_PIN, LOW);
            Serial.println("REVERB OFF");
        }

        switchLogic();
    }

    if (bFx3Bypass.fallingEdge())
    {
        state ^= S_DELAY;

        if (state & S_DELAY)
        {
            digitalWrite(LED_FX2_PIN, HIGH);
            Serial.println("DELAY ON");
        }
        else
        {
            digitalWrite(LED_FX2_PIN, LOW);
            Serial.println("DELAY OFF");
        }

        switchLogic();
    }

    if (bFx2Bypass.fallingEdge())
    {
        state ^= S_FLANGE;
        
        if (state & S_FLANGE)
        {
            digitalWrite(LED_FX3_PIN, HIGH);
            Serial.println("FLANGE ON");
        }
        else
        {
            digitalWrite(LED_FX3_PIN, LOW);
            Serial.println("FLANGE OFF");
        }
        
        switchLogic();
    }

    if (bFx1Bypass.fallingEdge())
    {
        state ^= S_CHORUS;
        
        if (state & S_CHORUS)
        {
            digitalWrite(LED_FX4_PIN, HIGH);
            Serial.println("CHORUS ON");
        }
        else
        {
            digitalWrite(LED_FX4_PIN, LOW);
            Serial.println("CHORUS OFF");
        }
        
        switchLogic();
    }
}

void initializeState()
{
    state = 0x0;
}

void zeroInputs(AudioMixer4 &mixerL, AudioMixer4 &mixerR)
{
    for (int i = 0; i != 4; ++i)
    {
        mixerL.gain(i, OFF);
        mixerR.gain(i, OFF);
    }
}

void zeroInputs(AudioMixer4 &mixerL, AudioMixer4 &mixerR, int start, int end)
{
    for (int i = start; i != end + 1; ++i)
    {
        mixerL.gain(i, OFF);
        mixerR.gain(i, OFF);
    }
}

void configurePins(void)
{
    //Potentiometers
    pinMode(GAININ_PIN, INPUT_PULLDOWN);
    pinMode(FILTER_PIN, INPUT_PULLDOWN);
    pinMode(WET_PIN, INPUT_PULLDOWN);
    pinMode(VOL_PIN, INPUT_PULLDOWN);
    //FX buttons
    pinMode(FX1_PIN, INPUT_PULLUP);
    pinMode(FX2_PIN, INPUT_PULLUP);
    pinMode(FX3_PIN, INPUT_PULLUP);
    pinMode(FX4_PIN, INPUT_PULLUP);
    //LED Pins
    pinMode(LED_FX1_PIN, OUTPUT);
    pinMode(LED_FX2_PIN, OUTPUT);
    pinMode(LED_FX3_PIN, OUTPUT);
    pinMode(LED_FX4_PIN, OUTPUT);
    pinMode(SYSTEM_LED, OUTPUT);
    
}

void configureAudioAdaptor(void)
{
    sgtl5000.enable();
    sgtl5000.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000.volume(vVolume);
}

void configureFilter(void)
{
    const float vFilterResonance = 0.707;

    filterL.frequency(vFilterFreq);
    filterR.frequency(vFilterFreq);
    filterL.resonance(vFilterResonance);
    filterR.resonance(vFilterResonance);
}

void configureReverb(void)
{
    const float vReverbSize = 0.5;
    const float vReverbDamping = 0.5;

    reverbL.roomsize(vReverbSize);
    reverbR.roomsize(vReverbSize);
    reverbL.damping(vReverbDamping);
    reverbR.damping(vReverbDamping);

    zeroInputs(mixerReverbInL, mixerReverbInR);
}

void configureDelay(void)
{
    const float vDelayTime = 375;
    const float vDelayFeedback = 0.7;

    mixerDelayOutL.gain(0, ON);
    mixerDelayOutR.gain(0, ON);
    mixerDelayOutL.gain(1, vDelayFeedback);
    mixerDelayOutR.gain(1, vDelayFeedback);
    delayL.delay(0, vDelayTime);
    delayR.delay(0, vDelayTime);

    zeroInputs(mixerDelayInL, mixerDelayInR);
}

void configureFlange(void)
{
    const int vOffset = FLANGE_DELAY_LENGTH/4;
    const int vDepth = FLANGE_DELAY_LENGTH/4;
    const double vDelayRate = 0.625;

    flangeR.begin(flangeDelayLineR, FLANGE_DELAY_LENGTH, vOffset, vDepth, vDelayRate);
    flangeL.begin(flangeDelayLineL, FLANGE_DELAY_LENGTH, vOffset, vDepth, vDelayRate);

    flangeL.voices(vOffset, vDepth, vDelayRate);
    flangeR.voices(vOffset, vDepth, vDelayRate);

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();

    zeroInputs(mixerFlangeInL, mixerFlangeInR);
}

void configureChorus(void)
{
    const float vChorusVoices = 4;

    if(!chorusL.begin(chorusDelayLineL, CHORUS_DELAY_LENGTH, vChorusVoices))
    {
        Serial.println("AudioEffectChorus - left channel begin failed");
        while(1);
    }
    if(!chorusR.begin(chorusDelayLineR, CHORUS_DELAY_LENGTH, vChorusVoices))
    {
        Serial.println("AudioEffectChorus - right channel begin failed");
        while(1);
    }

    chorusL.voices(vChorusVoices);
    chorusR.voices(vChorusVoices);

    zeroInputs(mixerChorusInL, mixerChorusInR);
}

void configureMixerFx(void)
{
    zeroInputs(mixerFxL, mixerFxR);
}

void configureMixerMaster(void)
{
    zeroInputs(mixerMasterL, mixerMasterR);

    setDryWetBalance();
}

void setGainIn(float value)
{
    gainInL.gain(value);
    gainInR.gain(value);
}

void setFilterFreq(float value)
{
    filterL.frequency(value);
    filterR.frequency(value);
}

void setDryWetBalance()
{
    if (state)
    {
        mixerMasterL.gain(0, 1.0 - vWet);
        mixerMasterR.gain(0, 1.0 - vWet);
        mixerMasterL.gain(1, vWet);
        mixerMasterR.gain(1, vWet);
    }
    else
    {
        mixerMasterL.gain(0, ON);
        mixerMasterR.gain(0, ON);
    }
}

void switchLogic(void)
{
    switch (state)
    {
        case 0x0:
        {
            Serial.println("Case: 0x0");
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);
            
            break;
        }
        case 0x1:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, ON);
            mixerReverbInR.gain(0, ON);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0x2:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, ON);
            mixerDelayInR.gain(0, ON);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0x3:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, ON);
            mixerDelayInR.gain(0, ON);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0x4:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, ON);
            mixerFxR.gain(1, ON);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0x5:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, ON);
            mixerReverbInR.gain(2, ON);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0x6:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0x7:
        {
            mixerChorusInL.gain(0, OFF);
            mixerChorusInR.gain(0, OFF);

            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0x8:
        {
            Serial.println("Case: 0x8");
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, ON);
            mixerFxR.gain(0, ON);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0x9:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, ON);
            mixerReverbInR.gain(1, ON);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0xA:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, ON);
            mixerDelayInR.gain(1, ON);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0xB:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, OFF);
            mixerFlangeInR.gain(1, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, ON);
            mixerDelayInR.gain(1, ON);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0xC:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, ON);
            mixerFlangeInR.gain(1, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, ON);
            mixerFxR.gain(1, ON);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0xD:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, ON);
            mixerFlangeInR.gain(1, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, ON);
            mixerReverbInR.gain(1, ON);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        case 0xE:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, ON);
            mixerFlangeInR.gain(1, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
            mixerFxL.gain(3, OFF);
            mixerFxR.gain(3, OFF);

            break;
        }
        case 0xF:
        {
            mixerChorusInL.gain(0, ON);
            mixerChorusInR.gain(0, ON);

            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);
            mixerFlangeInL.gain(1, ON);
            mixerFlangeInR.gain(1, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(1, OFF);
            mixerDelayInR.gain(1, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(1, OFF);
            mixerReverbInR.gain(1, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

            mixerFxL.gain(0, OFF);
            mixerFxR.gain(0, OFF);
            mixerFxL.gain(1, OFF);
            mixerFxR.gain(1, OFF);
            mixerFxL.gain(2, OFF);
            mixerFxR.gain(2, OFF);
            mixerFxL.gain(3, ON);
            mixerFxR.gain(3, ON);

            break;
        }
        default:
        {
            Serial.println("ERROR: INVALID STATE");
        }
    }
}

void printParameters(void)
{
    Serial.print("Gain In=");
    Serial.print(vGainIn / kGainInMax * 100.0);
    Serial.print("%, DryWet=");
    Serial.print(vWet * 100.0);
    Serial.print("%, Volume=");
    Serial.print(vVolume / kVolumeMax * 100.0);
    Serial.print("%, CPU Usage=");
    Serial.print(reverbL.processorUsage() + reverbR.processorUsage());
    Serial.println("%");

    Serial.flush();
    Serial.write(12);
}
