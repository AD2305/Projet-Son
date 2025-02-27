#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// 🎛️ Players audio (4 pistes)
AudioPlaySdWav player1;
AudioPlaySdWav player2;
AudioPlaySdWav player3;
AudioPlaySdWav player4;

// 🎛 Effets audio pour chaque player
AudioEffectFlange flanger1, flanger2, flanger3, flanger4;
AudioEffectGranular granular1, granular2, granular3, granular4;
AudioEffectChorus chorus1, chorus2, chorus3, chorus4;

// 🎚 Mixers individuels
AudioMixer4 mixer1;

// 🎚 Sortie audio
AudioOutputI2S audioOutput;
AudioControlSGTL5000 audioShield;


#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
short delayline[FLANGE_DELAY_LENGTH];




const int myInput = AUDIO_INPUT_LINEIN;

int s_idx = FLANGE_DELAY_LENGTH/4;
int s_depth = FLANGE_DELAY_LENGTH/4;
double s_freq = .5;

#define GRANULAR_MEMORY_SIZE 12800  // Mémoire pour l'effet granular
int16_t granularMemory[GRANULAR_MEMORY_SIZE];

#define CHORUS_DELAY_LENGTH (100 * AUDIO_BLOCK_SAMPLES)
short delayline2[CHORUS_DELAY_LENGTH];

// 🎚 Connexions audio
AudioConnection patchCord1(player1, 0, flanger1, 0);
AudioConnection patchCord2(flanger1, 0, granular1, 0);
AudioConnection patchCord3(granular1, 0, chorus1, 0);
AudioConnection patchCord4(chorus1, 0, mixer1, 0);

AudioConnection patchCord5(player2, 0, flanger2, 0);
AudioConnection patchCord6(flanger2, 0, granular2, 0);
AudioConnection patchCord7(granular2, 0, chorus2, 0);
AudioConnection patchCord8(chorus2, 0, mixer1, 1);

AudioConnection patchCord9(player3, 0, flanger3, 0);
AudioConnection patchCord10(flanger3, 0, granular3, 0);
AudioConnection patchCord11(granular3, 0, chorus3, 0);
AudioConnection patchCord12(chorus3, 0, mixer1, 2);

AudioConnection patchCord13(player4, 0, flanger4, 0);
AudioConnection patchCord14(flanger4, 0, granular4, 0);
AudioConnection patchCord15(granular4, 0, chorus4, 0);
AudioConnection patchCord16(chorus4, 0, mixer1, 3);

AudioConnection patchCord17(mixer1, 0, audioOutput, 0);
AudioConnection patchCord18(mixer1, 0, audioOutput, 1);

// 🎚 Potentiomètres
const int potFlanger = A1;  
const int potChorus = A3;   
const int potGranular = A2; 
const int potVolume = A0;   

// 📂 Carte SD
#define SDCARD_MOSI_PIN  11   
#define SDCARD_SCK_PIN   13  

void setup() {
    Serial.begin(9600);
    AudioMemory(50);

    // 🔊 Initialisation de la carte son
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_MIC);
    audioShield.micGain(10);
    audioShield.volume(0.4);

    // 📂 Initialisation de la carte SD
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    if (!SD.begin()) {
        Serial.println("Erreur : carte SD non détectée !");
        return;
    }

    // 🎛 Initialisation des effets
    
    flanger1.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular1.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular1.beginPitchShift(10000000);
    chorus1.begin(delayline2, CHORUS_DELAY_LENGTH, 2);

    flanger2.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular2.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular2.beginPitchShift(10000000);
    chorus2.begin(delayline2, CHORUS_DELAY_LENGTH, 2);

    flanger3.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular3.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular3.beginPitchShift(10000000);
    chorus3.begin(delayline2, CHORUS_DELAY_LENGTH, 2);

    flanger4.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular4.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular4.beginPitchShift(10000000);
    chorus4.begin(delayline2, CHORUS_DELAY_LENGTH, 2);

    



    // 🎚 Initialisation du mixer
    mixer1.gain(0, 0.8);

    // ▶ Démarrer les players (test automatique des pistes)
    player1.play("SDTEST1.WAV");
/*     player2.play("SDTEST2.WAV");
    player3.play("SDTEST3.WAV");
    player4.play("SDTEST4.WAV"); */
}

void loop() {
    // 🔄 Lecture des valeurs des potentiomètres
    int valFlanger = analogRead(potFlanger);
    int valChorus = analogRead(potChorus);
    int valGranular = analogRead(potGranular);
    int valVolume = analogRead(potVolume);
      

    // 🎛 Application des réglages sur les 4 players
    setFlanger(1, valFlanger);
    setFlanger(2, valFlanger);
    setFlanger(3, valFlanger);
    setFlanger(4, valFlanger);

    setChorus(1, valChorus);
    setChorus(2, valChorus);
    setChorus(3, valChorus);
    setChorus(4, valChorus);

    setGranular(1, valGranular);
    setGranular(2, valGranular);
    setGranular(3, valGranular);
    setGranular(4, valGranular);

    mixer1.gain(0, 1.0);

    // 🖥 Affichage des valeurs dans le moniteur série
    Serial.print("Flanger: "); Serial.print(valFlanger);
    Serial.print(" | Chorus: "); Serial.print(valChorus);
    Serial.print(" | Granular: "); Serial.print(valGranular);
/*     Serial.print(" | Volume: "); Serial.println(valVolume); */

    delay(500);

}

// 🎛 Réglage des effets
void setFlanger(int playerNumber, int potValue) {

    float speed = map(potValue, 0, 1023, 0, 10);
    float offset = map(potValue, 0, 1023, 0, 10);
    float s_idx = map(potValue, 0, 1023, 0, 10);
    
    
    if (playerNumber == 1) flanger1.voices(s_idx,speed,offset);
    else if (playerNumber == 2) flanger2.voices(FLANGE_DELAY_PASSTHRU, speed, offset);
    else if (playerNumber == 3) flanger3.voices(FLANGE_DELAY_PASSTHRU, speed, offset);
    else if (playerNumber == 4) flanger4.voices(FLANGE_DELAY_PASSTHRU, speed, offset);
}

void setChorus(int playerNumber, int potValue) {
    float voices = map(potValue, 0, 1023, 0, 100) ;

    if (playerNumber == 1) chorus1.voices(voices);
    else if (playerNumber == 2) chorus2.voices(voices);
    else if (playerNumber == 3) chorus3.voices(voices);
    else if (playerNumber == 4) chorus4.voices(voices);
}

void setGranular(int playerNumber, int potValue) {
    float speed = map(potValue, 0, 1023, 0.25, 8);

    if (playerNumber == 1) { granular1.setSpeed(speed); }
    else if (playerNumber == 2) { granular2.setSpeed(speed); }
    else if (playerNumber == 3) { granular3.setSpeed(speed); }
    else if (playerNumber == 4) { granular4.setSpeed(speed); }
}

// 🎚 Réglage du volume
void setVolume(int potValue) {
    /* float volumeLevel = map(potValue, 0, 1023, 0, 100) / 100.0; */
    mixer1.gain(0, 0.8);
   
} 


// juste pour le player n°1 fait les enchainement des effets, il est pas propore mais les principe est bon 
// faut choisir la bonne range de valeurs pour chaque effet et fixer des paramertres si besoin 

