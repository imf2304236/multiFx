#ifndef MULTIFX_AUDIOSYSTEM_H
#define MULTIFX_AUDIOSYSTEM_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;           //xy=106,327
AudioAmplifier           gainInL;           //xy=271,313
AudioAmplifier           gainInR;           //xy=271,343
AudioFilterStateVariable filterR;        //xy=437,729
AudioFilterStateVariable filterL;        //xy=469,56
AudioMixer4              mixerReverbInR;         //xy=616.0000076293945,759.6666641235352
AudioMixer4              mixerReverbInL;         //xy=644.0000076293945,85.66666412353516
AudioEffectFreeverbStereo reverbR;     //xy=802.3333053588867,759.6666679382324
AudioEffectFreeverbStereo reverbL;     //xy=832.3333053588867,85.66666793823242
AudioMixer4              mixerReverbPostR;         //xy=1003.3333053588867,772.6666641235352
AudioMixer4              mixerReverbPostL;         //xy=1031.3333053588867,99.66666412353516
AudioMixer4              mixerReverbOutR;         //xy=1295.3333053588867,483.66666412353516
AudioMixer4              mixerReverbOutL;         //xy=1303.3333053588867,189.66666412353516
AudioMixer4              mixerFxR;         //xy=1490.333251953125,462.3333320617676
AudioMixer4              mixerFxL;         //xy=1492.333251953125,168.33333206176758
AudioMixer4              mixerMasterR;        //xy=1697.9999237060547,374.33333587646484
AudioMixer4              mixerMasterL;         //xy=1700.9999237060547,265.33333587646484
AudioOutputI2S           audioOut;           //xy=1886,317
AudioConnection          patchCord1(audioIn, 0, gainInL, 0);
AudioConnection          patchCord2(audioIn, 1, gainInR, 0);
AudioConnection          patchCord3(gainInL, 0, filterL, 0);
AudioConnection          patchCord4(gainInL, 0, mixerReverbOutL, 0);
AudioConnection          patchCord5(gainInL, 0, mixerMasterL, 0);
AudioConnection          patchCord6(gainInR, 0, filterR, 0);
AudioConnection          patchCord7(gainInR, 0, mixerReverbOutR, 0);
AudioConnection          patchCord8(gainInR, 0, mixerMasterR, 0);
AudioConnection          patchCord9(filterR, 2, mixerReverbInR, 0);
AudioConnection          patchCord10(filterL, 2, mixerReverbInL, 0);
AudioConnection          patchCord11(mixerReverbInR, reverbR);
AudioConnection          patchCord12(mixerReverbInL, reverbL);
AudioConnection          patchCord13(reverbR, 0, mixerReverbPostL, 1);
AudioConnection          patchCord14(reverbR, 1, mixerReverbPostR, 1);
AudioConnection          patchCord15(reverbL, 0, mixerReverbPostL, 0);
AudioConnection          patchCord16(reverbL, 1, mixerReverbPostR, 0);
AudioConnection          patchCord17(mixerReverbPostR, 0, mixerReverbOutR, 1);
AudioConnection          patchCord18(mixerReverbPostL, 0, mixerReverbOutL, 1);
AudioConnection          patchCord19(mixerReverbOutR, 0, mixerFxR, 3);
AudioConnection          patchCord20(mixerReverbOutL, 0, mixerFxL, 3);
AudioConnection          patchCord21(mixerFxR, 0, mixerMasterR, 1);
AudioConnection          patchCord22(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord23(mixerMasterR, 0, audioOut, 1);
AudioConnection          patchCord24(mixerMasterL, 0, audioOut, 0);
AudioControlSGTL5000     sgtl5000;     //xy=104,28
// GUItool: end automatically generated code


#endif //MULTIFX_AUDIOSYSTEM_H
