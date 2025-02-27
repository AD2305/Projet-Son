#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// 🎛️ Objets audio
AudioInputI2S in;             // Entrée micro
AudioRecordQueue recorder;    // Enregistreur

AudioPlaySdWav           player1;
AudioPlaySdWav           player2;
AudioPlaySdWav           player3;
AudioPlaySdWav           player4;

// 🎛 Effets audio (Flanger, Granular, Chorus pour chaque player)
AudioEffectFlange        flanger1;
AudioEffectGranular      granular1;
AudioEffectChorus        chorus1;

AudioEffectFlange        flanger2;
AudioEffectGranular      granular2;
AudioEffectChorus        chorus2;

AudioEffectFlange        flanger3;
AudioEffectGranular      granular3;
AudioEffectChorus        chorus3;

AudioEffectFlange        flanger4;
AudioEffectGranular      granular4;
AudioEffectChorus        chorus4;

// 🎚 Mixers individuels pour chaque player
AudioMixer4              mixer1;

AudioOutputI2S audioOutput;      // Sortie audio
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

// 🎚 Assignation des connexions audio
AudioConnection patchCord1(player1, 0, flanger1, 0);    // Flanger pour player1
AudioConnection patchCord2(flanger1, 0, granular1, 0);  // Granular pour player1
AudioConnection patchCord3(granular1, 0, chorus1, 0);  // Chorus pour player1
AudioConnection patchCord4(chorus1, 0, mixer1, 0);     // Mixer1 pour player1

AudioConnection patchCord5(player2, 0, flanger2, 0);    // Flanger pour player2
AudioConnection patchCord6(flanger2, 0, granular2, 0);  // Granular pour player2
AudioConnection patchCord7(granular2, 0, chorus2, 0);  // Chorus pour player2
AudioConnection patchCord8(chorus2, 0, mixer1, 1);     // Mixer1 pour player2

AudioConnection patchCord9(player3, 0, flanger3, 0);    // Flanger pour player3
AudioConnection patchCord10(flanger3, 0, granular3, 0); // Granular pour player3
AudioConnection patchCord11(granular3, 0, chorus3, 0); // Chorus pour player3
AudioConnection patchCord12(chorus3, 0, mixer1, 2);    // Mixer1 pour player3

AudioConnection patchCord13(player4, 0, flanger4, 0);    // Flanger pour player4
AudioConnection patchCord14(flanger4, 0, granular4, 0);  // Granular pour player4
AudioConnection patchCord15(granular4, 0, chorus4, 0);  // Chorus pour player4
AudioConnection patchCord16(chorus4, 0, mixer1, 3);     // Mixer1 pour player4

AudioConnection patchCord17(mixer1, 0, audioOutput, 0); // Sortie mono gauche
AudioConnection patchCord18(mixer1, 0, audioOutput, 1); // Sortie mono droite

// Potentiomètres
const int potFlanger = A0;  // Contrôle du Flanger
const int potChorus = A1;   // Contrôle du Chorus
const int potGranular = A2; // Contrôle du Granular
const int potVolume = A3;   // Contrôle du volume principal

// 🛠 Boutons
const int button1 = 1;  // Joue le premier fichier
const int button2 = 2;  // Joue le deuxième fichier
const int button3 = 3;  
const int button4 = 4;  
const int stopButton = 5; // Stoppe tout
const int recordButton = 0; // Enregistre depuis le micro

// Variables de contrôle
int selectedMemory = 0;
const int recordDuration = 5;
bool isRecording = false;
int mixage[6] = {0, 0, 0, 0, 0, 0};

#define SDCARD_MOSI_PIN  11   
#define SDCARD_SCK_PIN   13  


void setup() {
    Serial.begin(9600);
    AudioMemory(40);

    // 🔊 Initialisation de la carte son
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_MIC);
    audioShield.micGain(10);
    audioShield.volume(0.8);

    
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);

    // Initialisation de la carte SD
    if (!SD.begin()) {
        Serial.println("Erreur : carte SD non détectée !");
        return ;
    }

    // Configuration des boutons
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(stopButton, INPUT_PULLUP);
    pinMode(recordButton, INPUT_PULLUP);

    // Réglage initial des effets
    flanger1.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular1.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular1.beginPitchShift(10000000);
    chorus1.begin(delayline2, CHORUS_DELAY_LENGTH, 1);

    flanger2.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular2.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular2.beginPitchShift(10000000);
    chorus2.begin(delayline2, CHORUS_DELAY_LENGTH, 1);

    flanger3.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular3.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular3.beginPitchShift(10000000);
    chorus3.begin(delayline2, CHORUS_DELAY_LENGTH, 1);

    flanger4.begin(delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
    granular4.begin(granularMemory, GRANULAR_MEMORY_SIZE);
    granular4.beginPitchShift(10000000);
    chorus4.begin(delayline2, CHORUS_DELAY_LENGTH, 1);

    // Initialisation des mixeurs (volume à 80% au départ)
    mixer1.gain(0, 0.8);
    mixer1.gain(1, 0.8);
    mixer1.gain(2, 0.8);
    mixer1.gain(3, 0.8);
}

void loop() {
    // Ajustement des effets avec les potentiomètres
    setFlanger(selectedMemory, analogRead(potFlanger));
    setChorus(selectedMemory, analogRead(potChorus));
    setGranular(selectedMemory, analogRead(potGranular));
    setVolume(selectedMemory, analogRead(potVolume));
    
    

    // Lecture des fichiers audio
    if (digitalRead(button1) == LOW) {
        Serial.println("SDTEST1.WAV");
        player1.play("SDTEST1.WAV");
        selectedMemory = 1;
        StopSounds(selectedMemory);
        Serial.println("Mémoire 1 sélectionnée");
        resetMixage(mixage,4,selectedMemory);
        mixage[selectedMemory] += 1 ;
        if (mixage[selectedMemory] % 2 == 0){
            selectedMemory = 5 ;
            ToutesPistes(selectedMemory);
        }
        delay(500);
    }

    if (digitalRead(button2) == LOW) {
        Serial.println("Lecture : SDTEST2.WAV ");
        player2.play("SDTEST2.WAV");
        selectedMemory = 2;
        StopSounds(selectedMemory);
        Serial.println("Mémoire 2 sélectionnée");
        resetMixage(mixage,4,selectedMemory);
        mixage[selectedMemory] += 1 ;
        if (mixage[selectedMemory] % 2 == 0){
            selectedMemory = 5 ;
            ToutesPistes(selectedMemory);
        }
        delay(500);
    }

    if (digitalRead(button3) == LOW) {
        Serial.println("Lecture : SDTEST3.WAV ");
        player3.play("SDTEST3.WAV");
        selectedMemory = 3;
        StopSounds(selectedMemory);
        Serial.println("Mémoire 3 sélectionnée");
        resetMixage(mixage,4,selectedMemory);
        mixage[selectedMemory] += 1 ;
        if (mixage[selectedMemory] % 2 == 0){
            selectedMemory = 5 ;
            ToutesPistes(selectedMemory);
        }
        delay(500);
    }

    if (digitalRead(button4) == LOW ) {
        Serial.println("Lecture : SDTEST4.WAV ");
        player4.play("SDTEST4.WAV");
        selectedMemory = 4;
        StopSounds(selectedMemory);
        Serial.println("Mémoire 4 sélectionnée");
        resetMixage(mixage,4,selectedMemory);
        mixage[selectedMemory] += 1 ;
        if (mixage[selectedMemory] % 2 == 0){
            selectedMemory = 5 ;
            ToutesPistes(selectedMemory);
            
        }
        delay(500);
    }

    // Arrêt des sons
    if (digitalRead(stopButton) == LOW) {
        if (player4.isPlaying()) {
            player1.stop();
            player2.stop();
            player3.stop();
            player4.stop();
            Serial.println("Arrêt des sons.");
            delay(300);
        } else {
            player1.play("SDTEST1.WAV");
            player2.play("SDTEST2.WAV");
            player3.play("SDTEST3.WAV");
            player4.play("SDTEST4.WAV");
            Serial.println("Reprends les sons.");
            delay(300);
        }
    }

    // Enregistrement du micro
    if (digitalRead(recordButton) == LOW && !isRecording) {
        Serial.println("Début enregistrement...");
        startRecording();
        delay(300);
    }

    if (isRecording) {
        recordAudio(selectedMemory);
    }



}

// Fonction pour démarrer l'enregistrement
void startRecording() {
    isRecording = true;
    recorder.begin();
}

// Fonction pour enregistrer depuis le micro
void recordAudio(int numeropiste) {
    //anaîs.
}

void setFlanger(int playerNumber, int potValue) {
    float offset = map(potValue, 0, 1000, 1, 100);
    float depth = map(potValue, 0, 1000, 1, 100);
    float delay = map(potValue, 0, 1000, 1, 100);
    
    if (playerNumber == 1) {
        flanger1.voices(offset, depth, delay);
    } else if (playerNumber == 2) {
        flanger2.voices(offset, depth, delay);
    } else if (playerNumber == 3) {
        flanger3.voices(offset, depth, delay);
    } else if (playerNumber == 4) {
        flanger4.voices(offset, depth, delay);
    }
}

void setChorus(int playerNumber, int potValue) {
    float voices = map(potValue, 0, 1000, 1, 10) ;
    
    if (playerNumber == 1) {
        chorus1.voices(voices);
        
    } else if (playerNumber == 2) {
        chorus2.voices(voices);
        
    } else if (playerNumber == 3) {
        chorus3.voices(voices);
        
    } else if (playerNumber == 4) {
        chorus4.voices(voices);
        
    }


}

void setGranular(int playerNumber, int potValue) {

    float speed = map(potValue, 0, 1000, 0.125, 8);
    
    if (playerNumber == 1) {
        granular1.setSpeed(speed);
    } else if (playerNumber == 2) {
        granular2.setSpeed(speed);
    } else if (playerNumber == 3) {
        granular3.setSpeed(speed);
    } else if (playerNumber == 4) {
        granular4.setSpeed(speed);
    }
}

void resetMixage(int mixage[], int taille, int index) {
    for (int i = 0; i < taille; i++) {
        mixage[i] = (i == index) ? 1 : 0;
    }
}

void StopSounds(int number) {
    if (number >= 1 && number <= 4) {
        if (number != 1) player1.stop();
        if (number != 2) player2.stop();
        if (number != 3) player3.stop();
        if (number != 4) player4.stop();
    }
    Serial.print("Arrêt de tous les sons sauf la piste ");
    Serial.println(number);
}

//  Ajuster le volume du player sélectionné
void setVolume(int playerNumber, int potValue) {
    float volumeLevel = map(analogRead(potVolume), 0, 1023, 0, 100) / 100.0;
  
    switch (playerNumber) {
      case 1: mixer1.gain(playerNumber - 1   , volumeLevel); break;
      case 2: mixer1.gain(playerNumber - 1  , volumeLevel); break;
      case 3: mixer1.gain(playerNumber - 1  , volumeLevel); break;
      case 4: mixer1.gain(playerNumber - 1  , volumeLevel); break;
      case 5: audioShield.volume(volumeLevel); break;
    }
}

void ToutesPistes(int a) {

    if (a == 5){
  
          player1.play("SDTEST1.WAV");
          player2.play("SDTEST2.WAV");
          player3.play("SDTEST3.WAV");
          player4.play("SDTEST4.WAV");
          Serial.println("Reprends les sons.");
          delay(300);
    }
}

    