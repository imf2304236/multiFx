#ifndef MULTIFX_AUDIOSYSTEM_H
#define MULTIFX_AUDIOSYSTEM_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;        //xy=124.666748046875,834.6666870117188
AudioAmplifier           gainInL;        //xy=289.666748046875,820.6666870117188
AudioAmplifier           gainInR;        //xy=289.666748046875,850.6666870117188
AudioFilterStateVariable filterR;        //xy=413.66680908203125,1096.0001220703125
AudioFilterStateVariable filterL;        //xy=491.00006103515625,142.6667022705078
AudioMixer4              mixerFlangeInR;         //xy=665,1003.3333129882812
AudioMixer4              mixerFlangeInL;         //xy=676.6666259765625,100
AudioMixer4              mixerDelayInR;  //xy=715.0000610351562,1193.6666870117188
AudioEffectFlange        flangeR;        //xy=786.6666870117188,1006.6668090820312
AudioMixer4              mixerDelayInL;  //xy=839.6666870117188,241.99998474121094
AudioEffectDelay         delayR;         //xy=897.0000610351562,1338.6666870117188
AudioMixer4              mixerDelayOutR; //xy=914.0000610351562,1213.6666870117188
AudioMixer4              mixerReverbInR; //xy=999.33349609375,1566.66650390625
AudioEffectDelay         delayL;         //xy=1019.6666870117188,404.99998474121094
AudioEffectFlange        flangeL;        //xy=1023.3335571289062,98.33332824707031
AudioMixer4              mixerDelayOutL; //xy=1052.6666870117188,264.99998474121094
AudioMixer4              mixerReverbInL; //xy=1137.6666870117188,626.9999847412109
AudioEffectFreeverb      reverbR;        //xy=1196.0001220703125,1682.9998779296875
AudioEffectFreeverb      reverbL;        //xy=1303.6666870117188,697.9999847412109
AudioMixer4              mixerFxL;       //xy=1586.666748046875,696.6666870117188
AudioMixer4              mixerFxR;       //xy=1627.3333333333335,1507.3333129882812
AudioMixer4              mixerMasterL;   //xy=1784.666748046875,831.6666870117188
AudioMixer4              mixerMasterR;   //xy=1785.666748046875,927.6666870117188
AudioOutputI2S           audioOut;       //xy=1940.666748046875,881.6666870117188
AudioConnection          patchCord1(audioIn, 0, gainInL, 0);
AudioConnection          patchCord2(audioIn, 1, gainInR, 0);
AudioConnection          patchCord3(gainInL, 0, filterL, 0);
AudioConnection          patchCord4(gainInL, 0, mixerMasterL, 0);
AudioConnection          patchCord5(gainInR, 0, filterR, 0);
AudioConnection          patchCord6(gainInR, 0, mixerMasterR, 0);
AudioConnection          patchCord7(filterR, 2, mixerDelayInR, 0);
AudioConnection          patchCord8(filterR, 2, mixerReverbInR, 0);
AudioConnection          patchCord9(filterR, 2, mixerFlangeInR, 0);
AudioConnection          patchCord10(filterL, 2, mixerDelayInL, 0);
AudioConnection          patchCord11(filterL, 2, mixerReverbInL, 0);
AudioConnection          patchCord12(filterL, 2, mixerFlangeInL, 0);
AudioConnection          patchCord13(mixerFlangeInR, flangeR);
AudioConnection          patchCord14(mixerFlangeInL, flangeL);
AudioConnection          patchCord15(mixerDelayInR, 0, mixerDelayOutR, 0);
AudioConnection          patchCord16(flangeR, 0, mixerDelayInR, 2);
AudioConnection          patchCord17(flangeR, 0, mixerFxR, 1);
AudioConnection          patchCord18(flangeR, 0, mixerReverbInR, 2);
AudioConnection          patchCord19(mixerDelayInL, 0, mixerDelayOutL, 0);
AudioConnection          patchCord20(delayR, 0, mixerDelayOutR, 1);
AudioConnection          patchCord21(mixerDelayOutR, delayR);
AudioConnection          patchCord22(mixerDelayOutR, 0, mixerFxR, 2);
AudioConnection          patchCord23(mixerDelayOutR, 0, mixerReverbInR, 3);
AudioConnection          patchCord24(mixerReverbInR, reverbR);
AudioConnection          patchCord25(delayL, 0, mixerDelayOutL, 1);
AudioConnection          patchCord26(flangeL, 0, mixerDelayInL, 2);
AudioConnection          patchCord27(flangeL, 0, mixerReverbInL, 2);
AudioConnection          patchCord28(flangeL, 0, mixerFxL, 1);
AudioConnection          patchCord29(mixerDelayOutL, delayL);
AudioConnection          patchCord30(mixerDelayOutL, 0, mixerFxL, 2);
AudioConnection          patchCord31(mixerDelayOutL, 0, mixerReverbInL, 3);
AudioConnection          patchCord32(mixerReverbInL, reverbL);
AudioConnection          patchCord33(reverbR, 0, mixerFxR, 3);
AudioConnection          patchCord34(reverbL, 0, mixerFxL, 3);
AudioConnection          patchCord35(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord36(mixerFxR, 0, mixerMasterR, 0);
AudioConnection          patchCord37(mixerMasterL, 0, audioOut, 0);
AudioConnection          patchCord38(mixerMasterR, 0, audioOut, 1);
AudioControlSGTL5000     sgtl5000;       //xy=107.33343505859375,53.00004577636719
// GUItool: end automatically generated code



#endif //MULTIFX_AUDIOSYSTEM_H
