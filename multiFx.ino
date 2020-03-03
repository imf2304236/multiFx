#include <avr/io.h>
#include <avr/interrupt.h>
#include <Bounce2.h>
#include "AudioSystem.h"

// Pins
#define GAININ_PIN  16
#define FILTER_PIN  17
#define WET_PIN     20
#define VOL_PIN     21
#define FX2_PIN     31
#define FX3_PIN     30
#define FX4_PIN     29
#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)

// Constants
#define ON  1.0
#define OFF 0.0

enum StateFlags {
    S_REVERB = 0x01,
    S_DELAY  = 0x02,
    S_FLANGE = 0x04,
    S_CHORUS = 0x08
};

const int kFilterFreqMax = 12000;
const float kGainInMax = 0.8;
const float kVolumeMax = 0.5;

elapsedMillis msec = 0;
const unsigned tUpdate = 1;
float vGainIn = kGainInMax;
float vVolume = kVolumeMax;
float vWet = 0.5;
int vFilterFreq = 10000;

uint8_t state = 0x0;

short flangeDelayLineL[FLANGE_DELAY_LENGTH];
short flangeDelayLineR[FLANGE_DELAY_LENGTH];

Bounce bFx2Bypass = Bounce(FX2_PIN,15);
Bounce bFx3Bypass = Bounce(FX3_PIN,15);
Bounce bFx4Bypass = Bounce(FX4_PIN,15);

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

    switchLogic();
}


void loop()
{
    if ((msec > tUpdate))
    {
        vGainIn = (float) analogRead(GAININ_PIN) / (1023.0 / kGainInMax);
        vFilterFreq = (float) analogRead(FILTER_PIN) / 1023.0 * kFilterFreqMax;
        vWet = (float) analogRead(WET_PIN) / (1023.0);
        vVolume = (float) analogRead(VOL_PIN) / (1023.0 / kVolumeMax);

        setGainIn(vGainIn);
        setFilterFreq(vFilterFreq);

        updateState();

        setDryWetBalance();

        sgtl5000.volume(vVolume);

        // printParameters();

        msec = 0;
    }
}


void updateState()
{
    bFx4Bypass.update();
    bFx3Bypass.update();
    bFx2Bypass.update();

    if (bFx4Bypass.fallingEdge())
    {
        state ^= S_REVERB;

        if (state & S_REVERB)
        {
            Serial.println("REVERB ON");
        }
        else
        {
            Serial.println("REVERB OFF");
        }

        switchLogic();
    }

    if (bFx3Bypass.fallingEdge())
    {
        state ^= S_DELAY;

        if (state & S_DELAY)
        {
            Serial.println("DELAY ON");
        }
        else
        {
            Serial.println("DELAY OFF");
        }

        switchLogic();
    }

    if (bFx2Bypass.fallingEdge())
    {
        state ^= S_FLANGE;
        
        if (state & S_FLANGE)
        {
            Serial.println("FLANGE ON");
        }
        else
        {
            Serial.println("FLANGE OFF");
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
    pinMode(GAININ_PIN, INPUT_PULLDOWN);
    pinMode(FILTER_PIN, INPUT_PULLDOWN);
    pinMode(WET_PIN, INPUT_PULLDOWN);
    pinMode(VOL_PIN, INPUT_PULLDOWN);
    pinMode(FX2_PIN, INPUT_PULLUP);
    pinMode(FX3_PIN, INPUT_PULLUP);
    pinMode(FX4_PIN, INPUT_PULLUP);
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
            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, ON);
            mixerReverbInR.gain(0, ON);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);

            mixerDelayInL.gain(0, ON);
            mixerDelayInR.gain(0, ON);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, OFF);
            mixerFlangeInR.gain(0, OFF);

            mixerDelayInL.gain(0, ON);
            mixerDelayInR.gain(0, ON);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

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
            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, OFF);
            mixerDelayInR.gain(2, OFF);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, ON);
            mixerReverbInR.gain(2, ON);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, OFF);
            mixerReverbInR.gain(3, OFF);

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
            mixerFlangeInL.gain(0, ON);
            mixerFlangeInR.gain(0, ON);

            mixerDelayInL.gain(0, OFF);
            mixerDelayInR.gain(0, OFF);
            mixerDelayInL.gain(2, ON);
            mixerDelayInR.gain(2, ON);

            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(2, OFF);
            mixerReverbInR.gain(2, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);

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
    Serial.print(", DryWet=");
    Serial.print(vWet * 100.0);
    Serial.print("%, Volume=");
    Serial.print(vVolume / kVolumeMax * 100.0);
    Serial.print("%, CPU Usage=");
    Serial.print(reverbL.processorUsage() + reverbR.processorUsage());
    Serial.println("%");

    Serial.flush();
    Serial.write(12);
}