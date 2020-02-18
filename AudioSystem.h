#ifndef MULTIFX_AUDIOSYSTEM_H
#define MULTIFX_AUDIOSYSTEM_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;        //xy=108.85714721679688,618.4285888671875
AudioAmplifier           gainInL;        //xy=273.8571472167969,604.4285888671875
AudioAmplifier           gainInR;        //xy=273.8571472167969,634.4285888671875
AudioFilterStateVariable filterR;        //xy=462.71435546875,1136.142822265625
AudioFilterStateVariable filterL;        //xy=471.85711669921875,56.00001525878906
AudioMixer4              mixerDelayInR;         //xy=640,802.8571166992188
AudioMixer4              mixerDelayInL;         //xy=718.5712890625,92.85714721679688
AudioEffectDelay         delayR;         //xy=822.8569946289062,947.1427612304688
AudioMixer4              mixerDelayOutR;         //xy=839.9998168945312,822.8569946289062
AudioEffectDelay         delayL;         //xy=898.5714111328125,255.71429443359375
AudioMixer4              mixerDelayOutL;         //xy=931.4284057617188,115.71427917480469
AudioMixer4              mixerReverbInL; //xy=1016.2860107421875,477.00006103515625
AudioMixer4              mixerReverbInR; //xy=1066.0001220703125,1150.4285888671875
AudioEffectFreeverbStereo reverbL;        //xy=1180.5714111328125,473.5715026855469
AudioEffectFreeverbStereo reverbR;        //xy=1226.2857666015625,1151.857177734375
AudioMixer4              mixerReverbPostL; //xy=1386.714599609375,500.4286193847656
AudioMixer4              mixerReverbPostR; //xy=1411.5712890625,1153.428466796875
AudioMixer4              mixerFxL;       //xy=1570.57177734375,480.8572082519531
AudioMixer4              mixerFxR;       //xy=1574.2857666015625,829.1427001953125
AudioMixer4              mixerMasterL;   //xy=1768.5716552734375,615.0000610351562
AudioMixer4              mixerMasterR;   //xy=1769.857421875,711.1427612304688
AudioOutputI2S           audioOut;       //xy=1924.571533203125,665.5714721679688
AudioConnection          patchCord1(audioIn, 0, gainInL, 0);
AudioConnection          patchCord2(audioIn, 1, gainInR, 0);
AudioConnection          patchCord3(gainInL, 0, filterL, 0);
AudioConnection          patchCord4(gainInL, 0, mixerMasterL, 0);
AudioConnection          patchCord5(gainInR, 0, filterR, 0);
AudioConnection          patchCord6(gainInR, 0, mixerMasterR, 0);
AudioConnection          patchCord7(filterR, 2, mixerReverbInR, 0);
AudioConnection          patchCord8(filterR, 2, mixerDelayInR, 0);
AudioConnection          patchCord9(filterL, 2, mixerReverbInL, 0);
AudioConnection          patchCord10(filterL, 2, mixerDelayInL, 0);
AudioConnection          patchCord11(mixerDelayInR, 0, mixerDelayOutR, 0);
AudioConnection          patchCord12(mixerDelayInL, 0, mixerDelayOutL, 0);
AudioConnection          patchCord13(delayR, 0, mixerDelayOutR, 1);
AudioConnection          patchCord14(mixerDelayOutR, delayR);
AudioConnection          patchCord15(mixerDelayOutR, 0, mixerReverbInR, 3);
AudioConnection          patchCord16(mixerDelayOutR, 0, mixerFxR, 2);
AudioConnection          patchCord17(delayL, 0, mixerDelayOutL, 1);
AudioConnection          patchCord18(mixerDelayOutL, delayL);
AudioConnection          patchCord19(mixerDelayOutL, 0, mixerFxL, 2);
AudioConnection          patchCord20(mixerDelayOutL, 0, mixerReverbInL, 3);
AudioConnection          patchCord21(mixerReverbInL, reverbL);
AudioConnection          patchCord22(mixerReverbInR, reverbR);
AudioConnection          patchCord23(reverbL, 0, mixerReverbPostR, 0);
AudioConnection          patchCord24(reverbL, 1, mixerReverbPostL, 0);
AudioConnection          patchCord25(reverbR, 0, mixerReverbPostR, 1);
AudioConnection          patchCord26(reverbR, 1, mixerReverbPostL, 1);
AudioConnection          patchCord27(mixerReverbPostL, 0, mixerFxL, 3);
AudioConnection          patchCord28(mixerReverbPostR, 0, mixerFxR, 3);
AudioConnection          patchCord29(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord30(mixerFxR, 0, mixerMasterR, 1);
AudioConnection          patchCord31(mixerMasterL, 0, audioOut, 0);
AudioConnection          patchCord32(mixerMasterR, 0, audioOut, 1);
AudioControlSGTL5000     sgtl5000;       //xy=104,28
// GUItool: end automatically generated code



#endif //MULTIFX_AUDIOSYSTEM_H
