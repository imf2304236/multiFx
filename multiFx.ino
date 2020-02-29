#include <avr/io.h>
#include <avr/interrupt.h>
#include "AudioSystem.h"

// Pins
#define GAININ_PIN  16
#define FILTER_PIN  17
#define WET_PIN     20
#define VOL_PIN     21
#define FX3_PIN     30
#define FX4_PIN     29

// Constants
#define ON  1.0
#define OFF 0.0

struct Effect
{
    int index = -1;
    volatile uint8_t isOn = LOW;
    AudioMixer4 *pMixerInL = NULL;
    AudioMixer4 *pMixerInR = NULL;
    AudioMixer4 *pMixerOutL = NULL;
    AudioMixer4 *pMixerOutR = NULL;
};

struct Effect eReverb = {
    3,
    LOW,
    &mixerReverbInL,
    &mixerReverbInR,
    NULL,
    NULL
};

struct Effect eDelay = {
    2,
    LOW,
    &mixerDelayInL,
    &mixerDelayInR,
    &mixerDelayOutL,
    &mixerDelayOutR
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
uint8_t numOfFxEnabled = 0;


void setup() {
    Serial.begin(9600);

    AudioMemory(500);   // Allocate memory for Audio connections

    configurePins();
    configureAudioAdaptor();

    setGainIn(vGainIn);

    numOfFxEnabled = uint8_t(eDelay.isOn) + uint8_t (eReverb.isOn);

    configureMixerMaster();
    configureMixerFx();
    configureReverb();
    configureDelay();

    configureISRs();
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

        setDryWetBalance();

        sgtl5000.volume(vVolume);

        // printParameters();

        msec = 0;
    }
}

void zeroInputs(struct Effect &effect)
{
    for (int i = 0; i != 4; ++i)
    {
        effect.pMixerInL->gain(i, OFF);
        effect.pMixerInR->gain(i, OFF);
    }
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
    pinMode(VOL_PIN, INPUT_PULLDOWN );
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
    const float vReverbSize = 0.9;
    const float vReverbDamping = 0.313;

    reverbL.roomsize(vReverbSize);
    reverbR.roomsize(vReverbSize);
    reverbL.damping(vReverbDamping);
    reverbR.damping(vReverbDamping);

    zeroInputs(mixerReverbInL, mixerReverbInR);

    if (eReverb.isOn)
    {
        if (eDelay.isOn)
        {
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);
        }
        else
        {
            mixerReverbInL.gain(0, ON);
            mixerReverbInR.gain(0, ON);
        }
    }
}

void configureDelay(void)
{
    const float vDelayTime = 375;
    const float vDelayFeedback = 0.5;

    mixerDelayOutL.gain(0, ON);
    mixerDelayOutR.gain(0, ON);
    mixerDelayOutL.gain(1, vDelayFeedback);
    mixerDelayOutR.gain(1, vDelayFeedback);
    delayL.delay(0, vDelayTime);
    delayR.delay(0, vDelayTime);

    zeroInputs(mixerDelayInL, mixerDelayInR);

    if (eDelay.isOn)
    {
        mixerDelayInL.gain(0, ON);
        mixerDelayInR.gain(0, ON);
    }
}

void configureMixerFx(void)
{
    zeroInputs(mixerFxL, mixerFxR);
    
    if (eReverb.isOn)
    {
        mixerFxL.gain(3, ON);
        mixerFxR.gain(3, ON);
    }
    else
    {
        if (eDelay.isOn)
        {
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
        }
    }
    
}

void configureMixerMaster(void)
{
    zeroInputs(mixerMasterL, mixerMasterR);

    if (numOfFxEnabled == 0)
    {
        mixerMasterL.gain(0, ON);
        mixerMasterR.gain(0, ON);
    }
    else
    {
        setDryWetBalance();
    }
}

void configureISRs(void)
{
    attachInterrupt(digitalPinToInterrupt(FX3_PIN), ISRDelayBypass, FALLING);
    attachInterrupt(digitalPinToInterrupt(FX4_PIN), ISRReverbBypass, FALLING);
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

void setDryWetBalance(struct Effect *pEffect, float vWet)
{
    pEffect->pMixerOutL->gain(0, 1.0 - vWet);
    pEffect->pMixerOutR->gain(0, 1.0 - vWet);
    pEffect->pMixerOutL->gain(1, vWet);
    pEffect->pMixerOutR->gain(1, vWet);
}

void setDryWetBalance()
{
    if (numOfFxEnabled)
    {
        mixerMasterL.gain(0, 1.0 - vWet);
        mixerMasterR.gain(0, 1.0 - vWet);
        mixerMasterL.gain(1, vWet);
        mixerMasterR.gain(1, vWet);
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

void ISRDelayBypass()
{
    eDelay.isOn = !(eDelay.isOn);

    if (eDelay.isOn)
    {
        Serial.println("DELAY ON");

        ++numOfFxEnabled;

        setDryWetBalance();

        // Serial Logic
        if (eReverb.isOn)
        {
            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);
        }
        else
        {
            mixerFxL.gain(2, ON);
            mixerFxR.gain(2, ON);
        }
        
        mixerDelayInL.gain(0, ON);
        mixerDelayInR.gain(0, ON);
    }
    else    // Delay switched OFF
    {
        Serial.println("DELAY OFF");

        --numOfFxEnabled;

        zeroInputs(eDelay);

        // Serial Logic
        if (eReverb.isOn)
        {
            mixerReverbInL.gain(0, ON);
            mixerReverbInR.gain(0, ON);
        }
        else
        {
            mixerMasterL.gain(0, ON);
            mixerMasterR.gain(0, ON);
        }
    }
    
}

void ISRReverbBypass()
{
    eReverb.isOn = !(eReverb.isOn);

    if (eReverb.isOn)   // Reverb Switched ON
    {
        Serial.println("REVERB ON");

        ++numOfFxEnabled;

        setDryWetBalance();

        mixerFxL.gain(eReverb.index, ON);
        mixerFxR.gain(eReverb.index, ON);

        // Serial Logic
        if (eDelay.isOn)
        {
            mixerFxL.gain(eDelay.index, OFF);
            mixerFxR.gain(eDelay.index, OFF);
            mixerReverbInL.gain(0, OFF);
            mixerReverbInR.gain(0, OFF);
            mixerReverbInL.gain(3, ON);
            mixerReverbInR.gain(3, ON);
        }
        else
        {
            mixerReverbInL.gain(0, ON);
            mixerReverbInR.gain(0, ON);
        }
    }
    else   // Reverb Switched OFF
    {
        Serial.println("REVERB OFF");

        --numOfFxEnabled;

        zeroInputs(eReverb);

        // Serial Logic
        if (eDelay.isOn)
        {
            mixerFxL.gain(eDelay.index, ON);
            mixerFxR.gain(eDelay.index, ON);
        }
        else
        {
            mixerMasterL.gain(0, ON);
            mixerMasterR.gain(0, ON);
        }
    }
}