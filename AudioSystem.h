#ifndef MULTIFX_AUDIOSYSTEM_H
#define MULTIFX_AUDIOSYSTEM_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;        //xy=138,750
AudioAmplifier           gainInL;        //xy=303,736
AudioAmplifier           gainInR;        //xy=303,766
AudioFilterStateVariable filterL;        //xy=461,310
AudioFilterStateVariable filterR;        //xy=513,1090
AudioMixer4              mixerChorusInL;         //xy=670,92
AudioMixer4              mixerChorusInR;         //xy=762.5,858.5
AudioMixer4              mixerFlangeInR; //xy=765,997
AudioMixer4              mixerDelayInR;  //xy=770,1265.7500610351562
AudioMixer4              mixerDelayInL;  //xy=774.25,390.75
AudioMixer4              mixerFlangeInL; //xy=788,250
AudioMixer4              mixerReverbInR; //xy=784,1513.7500610351562
AudioMixer4              mixerReverbInL; //xy=789.25,628
AudioEffectChorus        chorusL;        //xy=876.5,94
AudioEffectFlange        flangeR;        //xy=934,994
AudioEffectChorus        chorusR;        //xy=962.75,862.75
AudioEffectFreeverb      reverbL;        //xy=967.7500610351562,631.5
AudioEffectDelay         delayL;         //xy=970.5,511.25
AudioMixer4              mixerDelayOutL; //xy=976,411.25
AudioEffectDelay         delayR;         //xy=999.5,1378.2500610351562
AudioEffectFreeverb      reverbR;        //xy=1004,1524
AudioEffectFlange        flangeL;        //xy=1014.25,249
AudioMixer4              mixerDelayOutR; //xy=1014,1288.2500610351562
AudioMixer4              mixerFxL;       //xy=1534.4999389648438,338.0000305175781
AudioMixer4              mixerFxR;       //xy=1564,1159
AudioMixer4              mixerMasterL;   //xy=1827.6666259765625,717.666748046875
AudioMixer4              mixerMasterR;   //xy=1828.6666259765625,801.666748046875
AudioOutputI2S           audioOut;       //xy=1983.6666259765625,767.666748046875
AudioConnection          patchCord1(audioIn, 0, gainInL, 0);
AudioConnection          patchCord2(audioIn, 1, gainInR, 0);
AudioConnection          patchCord3(gainInL, 0, filterL, 0);
AudioConnection          patchCord4(gainInL, 0, mixerMasterL, 0);
AudioConnection          patchCord5(gainInR, 0, filterR, 0);
AudioConnection          patchCord6(gainInR, 0, mixerMasterR, 0);
AudioConnection          patchCord7(filterL, 2, mixerChorusInL, 0);
AudioConnection          patchCord8(filterL, 2, mixerFlangeInL, 0);
AudioConnection          patchCord9(filterL, 2, mixerDelayInL, 0);
AudioConnection          patchCord10(filterL, 2, mixerReverbInL, 0);
AudioConnection          patchCord11(filterR, 2, mixerFlangeInR, 0);
AudioConnection          patchCord12(filterR, 2, mixerDelayInR, 0);
AudioConnection          patchCord13(filterR, 2, mixerReverbInR, 0);
AudioConnection          patchCord14(filterR, 2, mixerChorusInR, 0);
AudioConnection          patchCord15(mixerChorusInL, chorusL);
AudioConnection          patchCord16(mixerChorusInR, chorusR);
AudioConnection          patchCord17(mixerFlangeInR, flangeR);
AudioConnection          patchCord18(mixerDelayInR, 0, mixerDelayOutR, 0);
AudioConnection          patchCord19(mixerDelayInL, 0, mixerDelayOutL, 0);
AudioConnection          patchCord20(mixerFlangeInL, flangeL);
AudioConnection          patchCord21(mixerReverbInR, reverbR);
AudioConnection          patchCord22(mixerReverbInL, reverbL);
AudioConnection          patchCord23(chorusL, 0, mixerFlangeInL, 1);
AudioConnection          patchCord24(chorusL, 0, mixerDelayInL, 1);
AudioConnection          patchCord25(chorusL, 0, mixerReverbInL, 1);
AudioConnection          patchCord26(chorusL, 0, mixerFxL, 0);
AudioConnection          patchCord27(flangeR, 0, mixerDelayInR, 2);
AudioConnection          patchCord28(flangeR, 0, mixerReverbInR, 2);
AudioConnection          patchCord29(flangeR, 0, mixerFxR, 1);
AudioConnection          patchCord30(chorusR, 0, mixerFlangeInR, 1);
AudioConnection          patchCord31(chorusR, 0, mixerDelayInR, 1);
AudioConnection          patchCord32(chorusR, 0, mixerReverbInR, 1);
AudioConnection          patchCord33(chorusR, 0, mixerFxR, 0);
AudioConnection          patchCord34(reverbL, 0, mixerFxL, 3);
AudioConnection          patchCord35(delayL, 0, mixerDelayOutL, 1);
AudioConnection          patchCord36(mixerDelayOutL, delayL);
AudioConnection          patchCord37(mixerDelayOutL, 0, mixerReverbInL, 3);
AudioConnection          patchCord38(mixerDelayOutL, 0, mixerFxL, 2);
AudioConnection          patchCord39(delayR, 0, mixerDelayOutR, 1);
AudioConnection          patchCord40(reverbR, 0, mixerFxR, 3);
AudioConnection          patchCord41(flangeL, 0, mixerDelayInL, 2);
AudioConnection          patchCord42(flangeL, 0, mixerReverbInL, 2);
AudioConnection          patchCord43(flangeL, 0, mixerFxL, 1);
AudioConnection          patchCord44(mixerDelayOutR, delayR);
AudioConnection          patchCord45(mixerDelayOutR, 0, mixerReverbInR, 3);
AudioConnection          patchCord46(mixerDelayOutR, 0, mixerFxR, 2);
AudioConnection          patchCord47(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord48(mixerFxR, 0, mixerMasterR, 1);
AudioConnection          patchCord49(mixerMasterL, 0, audioOut, 0);
AudioConnection          patchCord50(mixerMasterR, 0, audioOut, 1);
AudioControlSGTL5000     sgtl5000;       //xy=99,55
// GUItool: end automatically generated code




#endif //MULTIFX_AUDIOSYSTEM_H
