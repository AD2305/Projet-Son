#include "Init_Audio_Sys.h"

AudioInputI2S            in;  
AudioOutputI2S           out;
AudioControlSGTL5000 audioShield;

AudioPlaySdWav           player1;
AudioPlaySdWav           player2;
AudioPlaySdWav           player3;
AudioPlaySdWav           player4;

AudioRecordQueue queue1;
AudioMixer4 mixer1;

AudioConnection patchCord1(in, 0, queue1, 0); 
AudioConnection patchCord2(mixer1, 0, out, 0);  
AudioConnection patchCord3(mixer1, 0, out, 1);
AudioConnection patchCord4(player1,0, mixer1, 0);
AudioConnection patchCord5(player2,0, mixer1, 1);
AudioConnection patchCord6(player3,0, mixer1, 2);
AudioConnection patchCord7(player4,0, mixer1, 3);

