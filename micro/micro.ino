#include <Audio.h>

AudioInputI2S in;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(in,0,out,0);
AudioConnection patchCord1(in,0,out,1);

bool on = false;

void setup() {
  Serial.begin(9600);
  AudioMemory(6);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(10); // in dB
  audioShield.volume(1);
  pinMode(0, INPUT);
}

void loop() {
    if (digitalRead(0)) {
        on = !on; // Change l'état à chaque pression
        delay(200); // Anti-rebond (évite des changements trop rapides)
    }

    if (on) {
        audioShield.micGain(10); // Active le micro
    } else {
        audioShield.micGain(0); // Désactive le micro
    }
}
