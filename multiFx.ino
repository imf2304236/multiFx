#include <Audio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>

#define DRYWET_PIN  20
#define FILTER_PIN  17
#define FX1_PIN     32
#define FX2_PIN     31
#define FX3_PIN     30
#define FX4_PIN     29
#define GAININ_PIN  16
#define VOL_PIN     21
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES)
#define FLANGE_DELAY_LENGTH (12*AUDIO_BLOCK_SAMPLES)

void configureAudioAdaptor(void);
void configurePins(void);
void setDryWetBalance(void);
void initalizeSignalFlow(void);
void printParameters(void);


// GUItool: begin automatically generated code
AudioInputI2S            audioIn;        //xy=134.111083984375,1026.8887939453125
AudioAmplifier           gainInL;        //xy=292.111083984375,1000.8887939453125
AudioAmplifier           gainInR;        //xy=292.111083984375,1065.8887939453125
AudioFilterStateVariable filterR;        //xy=418,1434
AudioFilterStateVariable filterL;        //xy=475,244
AudioMixer4              mixerReverbInR; //xy=717.4444580078125,2022.0001220703125
AudioMixer4              mixerReverbInL; //xy=731.666748046875,664.3055419921875
AudioMixer4              mixerChorusInL; //xy=739,43.75
AudioMixer4              mixerDelayInR;  //xy=732.2222290039062,1807.6666870117188
AudioMixer4              mixerFlangeInR; //xy=736.3333129882812,1557.8889465332031
AudioMixer4              mixerDelayInL;  //xy=742.1944274902344,320.7222595214844
AudioMixer4              mixerChorusInR; //xy=746.2222290039062,1371.6666870117188
AudioMixer4              mixerFlangeInL; //xy=755.1112060546875,169.1388702392578
AudioEffectFreeverbStereo reverbR;        //xy=886.7777709960938,2023.333251953125
AudioEffectFreeverbStereo reverbL;        //xy=921.9166259765625,659.9722900390625
AudioMixer4              mixerReverbPostR; //xy=1121.6666870117188,1997.0003051757812
AudioMixer4              mixerReverbPostL; //xy=1133,677.5
AudioEffectDelay         delayL;         //xy=1159.5,429.916748046875
AudioMixer4              mixerDelayFeedbackR; //xy=1154.4443969726562,1831.2223510742188
AudioMixer4              mixerDelayFeedbackL; //xy=1168.9166870117188,341.97222900390625
AudioEffectFlange        flangeL;        //xy=1194.22216796875,167.27774047851562
AudioEffectChorus        chorusR;        //xy=1189.6666259765625,1375.3333740234375
AudioEffectFlange        flangeR;        //xy=1193.7777099609375,1562.6666259765625
AudioEffectDelay         delayR;         //xy=1201.9999389648438,1741.1111450195312
AudioEffectChorus        chorusL;        //xy=1212.416748046875,45.055564880371094
AudioMixer4              mixerDelayOutL;         //xy=1488.5556640625,869.333251953125
AudioMixer4              mixerChorusOutL;         //xy=1492.2220458984375,750.3333740234375
AudioMixer4              mixerFlangeOutL;         //xy=1492.000244140625,812.1110229492188
AudioMixer4              mixerReverbOutL;         //xy=1493,930.1112670898438
AudioMixer4              mixerChorusOutR;         //xy=1519.1115112304688,1153.5554504394531
AudioMixer4              mixerFlangeOutR;         //xy=1520.222412109375,1215.7777099609375
AudioMixer4              mixerReverbOutR;         //xy=1520.2222900390625,1336.8888549804688
AudioMixer4              mixerDelayOutR;         //xy=1523.5557657877605,1278.0001695421008
AudioMixer4              mixerFxL;   //xy=1811.0555419921875,850.3057250976562
AudioMixer4              mixerFxR;   //xy=1863.1109619140625,1234.22216796875
AudioMixer4              mixerMasterL;         //xy=2041,1038
AudioMixer4              mixerMasterR;        //xy=2043,1102
AudioOutputI2S           audioOut;       //xy=2253.97216796875,1066.111083984375
AudioConnection          patchCord1(audioIn, 0, gainInL, 0);
AudioConnection          patchCord2(audioIn, 1, gainInR, 0);
AudioConnection          patchCord3(gainInL, 0, filterL, 0);
AudioConnection          patchCord4(gainInL, 0, mixerChorusOutL, 0);
AudioConnection          patchCord5(gainInL, 0, mixerFlangeOutL, 0);
AudioConnection          patchCord6(gainInL, 0, mixerDelayOutL, 0);
AudioConnection          patchCord7(gainInL, 0, mixerReverbOutL, 0);
AudioConnection          patchCord8(gainInL, 0, mixerMasterL, 0);
AudioConnection          patchCord9(gainInR, 0, filterR, 0);
AudioConnection          patchCord10(gainInR, 0, mixerChorusOutR, 0);
AudioConnection          patchCord11(gainInR, 0, mixerFlangeOutR, 0);
AudioConnection          patchCord12(gainInR, 0, mixerDelayOutR, 0);
AudioConnection          patchCord13(gainInR, 0, mixerReverbOutR, 0);
AudioConnection          patchCord14(gainInR, 0, mixerMasterR, 0);
AudioConnection          patchCord15(filterR, 2, mixerChorusInR, 0);
AudioConnection          patchCord16(filterR, 2, mixerFlangeInR, 0);
AudioConnection          patchCord17(filterR, 2, mixerDelayInR, 0);
AudioConnection          patchCord18(filterR, 2, mixerReverbInR, 0);
AudioConnection          patchCord19(filterL, 2, mixerReverbInL, 0);
AudioConnection          patchCord20(filterL, 2, mixerDelayInL, 0);
AudioConnection          patchCord21(filterL, 2, mixerFlangeInL, 0);
AudioConnection          patchCord22(filterL, 2, mixerChorusInL, 0);
AudioConnection          patchCord23(mixerReverbInR, reverbR);
AudioConnection          patchCord24(mixerReverbInL, reverbL);
AudioConnection          patchCord25(mixerChorusInL, chorusL);
AudioConnection          patchCord26(mixerDelayInR, 0, mixerDelayFeedbackR, 0);
AudioConnection          patchCord27(mixerFlangeInR, flangeR);
AudioConnection          patchCord28(mixerDelayInL, 0, mixerDelayFeedbackL, 0);
AudioConnection          patchCord29(mixerChorusInR, chorusR);
AudioConnection          patchCord30(mixerFlangeInL, flangeL);
AudioConnection          patchCord31(reverbR, 0, mixerReverbPostL, 1);
AudioConnection          patchCord32(reverbR, 1, mixerReverbPostR, 1);
AudioConnection          patchCord33(reverbL, 0, mixerReverbPostL, 0);
AudioConnection          patchCord34(reverbL, 1, mixerReverbPostR, 0);
AudioConnection          patchCord35(mixerReverbPostR, 0, mixerReverbOutR, 1);
AudioConnection          patchCord36(mixerReverbPostL, 0, mixerReverbOutL, 1);
AudioConnection          patchCord37(delayL, 0, mixerDelayFeedbackL, 1);
AudioConnection          patchCord38(delayL, 0, mixerDelayOutL, 1);
AudioConnection          patchCord39(mixerDelayFeedbackR, delayR);
AudioConnection          patchCord40(mixerDelayFeedbackL, delayL);
AudioConnection          patchCord41(flangeL, 0, mixerFlangeOutL, 1);
AudioConnection          patchCord42(chorusR, 0, mixerChorusOutR, 1);
AudioConnection          patchCord43(flangeR, 0, mixerFlangeOutR, 1);
AudioConnection          patchCord44(delayR, 0, mixerDelayOutR, 1);
AudioConnection          patchCord45(delayR, 0, mixerDelayFeedbackR, 1);
AudioConnection          patchCord46(chorusL, 0, mixerChorusOutL, 1);
AudioConnection          patchCord47(mixerDelayOutL, 0, mixerReverbInL, 3);
AudioConnection          patchCord48(mixerDelayOutL, 0, mixerFxL, 2);
AudioConnection          patchCord49(mixerChorusOutL, 0, mixerFlangeInL, 1);
AudioConnection          patchCord50(mixerChorusOutL, 0, mixerDelayInL, 1);
AudioConnection          patchCord51(mixerChorusOutL, 0, mixerReverbInL, 1);
AudioConnection          patchCord52(mixerChorusOutL, 0, mixerFxL, 0);
AudioConnection          patchCord53(mixerFlangeOutL, 0, mixerDelayInL, 2);
AudioConnection          patchCord54(mixerFlangeOutL, 0, mixerReverbInL, 2);
AudioConnection          patchCord55(mixerFlangeOutL, 0, mixerFxL, 1);
AudioConnection          patchCord56(mixerReverbOutL, 0, mixerFxL, 3);
AudioConnection          patchCord57(mixerChorusOutR, 0, mixerFxR, 0);
AudioConnection          patchCord58(mixerChorusOutR, 0, mixerFlangeInR, 1);
AudioConnection          patchCord59(mixerChorusOutR, 0, mixerDelayInR, 1);
AudioConnection          patchCord60(mixerChorusOutR, 0, mixerReverbInR, 1);
AudioConnection          patchCord61(mixerFlangeOutR, 0, mixerFxR, 1);
AudioConnection          patchCord62(mixerFlangeOutR, 0, mixerDelayInR, 2);
AudioConnection          patchCord63(mixerFlangeOutR, 0, mixerReverbInR, 2);
AudioConnection          patchCord64(mixerReverbOutR, 0, mixerFxR, 3);
AudioConnection          patchCord65(mixerDelayOutR, 0, mixerFxR, 2);
AudioConnection          patchCord66(mixerDelayOutR, 0, mixerReverbInR, 3);
AudioConnection          patchCord67(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord68(mixerFxR, 0, mixerMasterR, 1);
AudioConnection          patchCord69(mixerMasterL, 0, audioOut, 0);
AudioConnection          patchCord70(mixerMasterR, 0, audioOut, 1);
AudioControlSGTL5000     sgtl5000;       //xy=72,28
// GUItool: end automatically generated code


const float kGainInMax = 0.5;
const float kVolumeMax = 0.8;


elapsedMillis msec = 0;
const unsigned tUpdate = 1;
float vGainIn = kGainInMax;
float vVolume = kVolumeMax;


void setup() {  
  AudioNoInterrupts();
  
  Serial.begin(9600);

  AudioMemory(400);   // Audio connections require memory to work.  For more
                      // detailed information, see the MemoryAndCpuUsage example
  
  configureAudioAdaptor();
  configurePins();
  
  setDryWetBalance();
  initalizeSignalFlow();
  
  AudioInterrupts();
}


void loop()
{ 
  if ((msec > tUpdate))
  {
    vGainIn = (float) analogRead(GAININ_PIN) / (1023.0 / kGainInMax);
    vVolume = (float) analogRead(VOL_PIN) / (1023.0 / kVolumeMax);
//    vDryWet = (float) analogRead(DRYWET_PIN) / (1023.0);
    
    gainInL.gain(vGainIn);
    gainInR.gain(vGainIn);

    sgtl5000.volume(vVolume);
    
    printParameters();

    msec = 0;
  }
}

void configureAudioAdaptor(void)
{
  sgtl5000.enable();
  sgtl5000.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000.volume(vVolume);

  gainInL.gain(vGainIn);
  gainInR.gain(vGainIn);
}

void configurePins(void)
{
//  pinMode(DRYWET_PIN, INPUT_PULLDOWN);
//  pinMode(FX1_PIN, INPUT_PULLUP);
//  pinMode(FX2_PIN, INPUT_PULLUP);
//  pinMode(FX3_PIN, INPUT_PULLUP);
//  pinMode(FX4_PIN, INPUT_PULLUP);
  pinMode(GAININ_PIN, INPUT_PULLDOWN);
  pinMode(VOL_PIN, INPUT_PULLDOWN );
}

void setDryWetBalance(void)
{
//  for (int i = 0; i != 4; ++i)
//  {
//    effects[i].pMixerOutL->gain(0, vDryWet);
//    effects[i].pMixerOutR->gain(0, vDryWet);
//    effects[i].pMixerOutL->gain(1, 1.0 - vDryWet);
//    effects[i].pMixerOutR->gain(1, 1.0 - vDryWet);
//  }
  mixerMasterL.gain(0, 1.0);
  mixerMasterR.gain(0, 1.0);
}

void initalizeSignalFlow(void)
{
//  int indexOfFirstActiveEffect = getIndexOfNextActiveEffect(0);
//  if (indexOfFirstActiveEffect > -1)
//  {
//    effects[indexOfFirstActiveEffect].pMixerInL->gain(0, 1.0);
//    effects[indexOfFirstActiveEffect].pMixerInR->gain(0, 1.0);
//
//    int indexOfNextActiveEffect = indexOfFirstActiveEffect;
//    int indexOfLastActiveEffect = indexOfFirstActiveEffect;
//    while (indexOfNextActiveEffect != -1)
//    {
//      indexOfLastActiveEffect = indexOfNextActiveEffect;
//      indexOfNextActiveEffect = getIndexOfNextActiveEffect(indexOfLastActiveEffect);
//
//      if (indexOfNextActiveEffect != -1)
//      {
//        int inputIndexOfLastActiveEffect = indexOfLastActiveEffect + 1;
//        effects[indexOfNextActiveEffect].pMixerInL->gain(inputIndexOfLastActiveEffect, 1.0);
//        effects[indexOfNextActiveEffect].pMixerInR->gain(inputIndexOfLastActiveEffect, 1.0);
//      }
//    }
//
//    mixerFxL.gain(indexOfLastActiveEffect, 1.0);
//    mixerFxR.gain(indexOfLastActiveEffect, 1.0);
//
//    mixerMasterL.gain(0, 0.0);  // Turn Dry mix OFF
//    mixerMasterR.gain(0, 0.0);
//    mixerMasterL.gain(1, 1.0);  // Turn Wet mix ON
//    mixerMasterR.gain(1, 1.0);
//  }
//  else  // no active effects
//  {
//    mixerMasterL.gain(0, 1.0);  // Turn Dry mix ON
//    mixerMasterR.gain(0, 1.0);
//    mixerMasterL.gain(1, 0.0);  // Turn Wet mix OFF
//    mixerMasterR.gain(1, 0.0);
//  }

  setDryWetBalance();
}

void printParameters(void)
{ 
  Serial.print("Gain In=");
  Serial.print(vGainIn / kGainInMax * 100.0);
//  Serial.print(", Filter Freq=");
//  Serial.print(vFilterFreq);
//  Serial.print(", DryWet=");
//  Serial.print(vDryWet * 100.0);
//  Serial.print("%, Volume=");
  Serial.print(", Volume=");
  Serial.print(vVolume / kVolumeMax * 100.0);
  Serial.print("%, CPU Usage=");
  Serial.print(reverbL.processorUsage() + reverbR.processorUsage());
  Serial.println("%");

  Serial.flush();
  Serial.write(12);
}
