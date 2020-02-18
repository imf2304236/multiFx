#include <avr/io.h>
#include <avr/interrupt.h>
#include "AudioSystem.h"

#define GAININ_PIN  16
#define FILTER_PIN  17
#define WET_PIN     20
#define VOL_PIN     21
#define FX4_PIN     29

struct Effect
{
    int index = -1;
    volatile uint8_t isOn = LOW;
    AudioMixer4 *pMixerInL = NULL;
    AudioMixer4 *pMixerInR = NULL;
    AudioMixer4 *pMixerOutL = NULL;
    AudioMixer4 *pMixerOutR = NULL;
} eReverb;

struct Effect eReverb = {
    3,
    HIGH,
    &mixerReverbInL,
    &mixerReverbInR,
    &mixerReverbOutL,
    &mixerReverbOutR
};

const int kFilterFreqMax = 12000;
const float kGainInMax = 0.8;
const float kVolumeMax = 0.5;
float vGainInReverb = 0.0;

elapsedMillis msec = 0;
const unsigned tUpdate = 1;
float vGainIn = kGainInMax;
float vVolume = kVolumeMax;
float vWet = 0.5;
int vFilterFreq = 10000;


void setup() {
    Serial.begin(9600);

    AudioMemory(400);   // Allocate memory for Audio connections

    configurePins();
    configureAudioAdaptor();

    setGainIn(vGainIn);

    zeroInputs(mixerMasterL, mixerMasterR);
    zeroInputs(mixerFxL, mixerFxR);

    configureReverb();

    initializeDryWetSwitch();

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

        setDryWetBalance(&eReverb, vWet);

        sgtl5000.volume(vVolume);

        printParameters();

        msec = 0;
    }
}

void zeroInputs(AudioMixer4 &mixerL, AudioMixer4 &mixerR)
{
    for (int i = 0; i != 4; ++i)
    {
        mixerL.gain(i, 0.0);
        mixerR.gain(i, 0.0);
    }
}

void zeroInputs(AudioMixer4 &mixerL, AudioMixer4 &mixerR, int start, int end)
{
    for (int i = start; i != end + 1; ++i)
    {
        mixerL.gain(i, 0.0);
        mixerR.gain(i, 0.0);
    }
}

void configurePins(void)
{
    pinMode(GAININ_PIN, INPUT_PULLDOWN);
    pinMode(FILTER_PIN, INPUT_PULLDOWN);
    pinMode(WET_PIN, INPUT_PULLDOWN);
    pinMode(VOL_PIN, INPUT_PULLDOWN );
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

    zeroInputs(mixerReverbOutL, mixerReverbOutR);
    zeroInputs(mixerReverbInL, mixerReverbInR);

    mixerReverbPostL.gain(0, 1.0);
    mixerReverbPostR.gain(0, 1.0);
    mixerReverbPostL.gain(1, 1.0);
    mixerReverbPostR.gain(1, 1.0);

    zeroInputs(mixerReverbPostL, mixerReverbPostR, 2, 3);

    setDryWetBalance(&eReverb, vWet);

    if (eReverb.isOn)
    {
        mixerReverbInL.gain(0, 1.0);
        mixerReverbInR.gain(0, 1.0);

        mixerFxL.gain(eReverb.index, 1.0);
        mixerFxR.gain(eReverb.index, 1.0);
    }
}

void initializeDryWetSwitch(void)
{
    if (eReverb.isOn)
    {
        mixerMasterL.gain(0, 0.0);
        mixerMasterR.gain(0, 0.0);
        mixerMasterL.gain(1, 1.0);
        mixerMasterR.gain(1, 1.0);
    }
    else
    {
        mixerMasterL.gain(0, 1.0);
        mixerMasterR.gain(0, 1.0);
        mixerMasterL.gain(1, 0.0);
        mixerMasterR.gain(1, 0.0);
    }

    mixerMasterL.gain(2, 0.0);
    mixerMasterR.gain(2, 0.0);
    mixerMasterL.gain(3, 0.0);
    mixerMasterR.gain(3, 0.0);
}

void configureISRs(void)
{
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

void ISRReverbBypass()
{
    Serial.println("BYPASS REVERB");

    eReverb.isOn = !(eReverb.isOn);

    if (eReverb.isOn)   // Reverb Switched ON
    {
        mixerMasterL.gain(0, 0.0);  // Turn Dry mix OFF
        mixerMasterR.gain(0, 0.0);
        mixerMasterL.gain(1, 1.0);  // Turn Wet mix ON
        mixerMasterR.gain(1, 1.0);

        mixerFxL.gain(eReverb.index, 1.0);
        mixerFxR.gain(eReverb.index, 1.0);

        eReverb.pMixerInL->gain(0, 1.0);    // Turn Filter input ON
        eReverb.pMixerInR->gain(0, 1.0);
    }
    else   // Reverb Switched OFF
    {
        mixerMasterL.gain(0, 1.0);  // Turn Dry mix ON
        mixerMasterR.gain(0, 1.0);
        mixerMasterL.gain(1, 0.0);  // Turn Wet mix OFF
        mixerMasterR.gain(1, 0.0);

        mixerFxL.gain(eReverb.index, 0.0);
        mixerFxR.gain(eReverb.index, 0.0);

        eReverb.pMixerInL->gain(0, 0.0);    // Turn Filter input OFF
        eReverb.pMixerInR->gain(0, 0.0);
    }
}