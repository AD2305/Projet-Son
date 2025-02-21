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

// 🎛 Effets audio (1 par player)
/* AudioEffectReverb        reverb1;
AudioEffectReverb        reverb2;
AudioEffectReverb        reverb3;
AudioEffectReverb        reverb4; */
/* 
AudioEffectDelay         delay1;
AudioEffectDelay         delay2;
AudioEffectDelay         delay3;
AudioEffectDelay         delay4; */

// 🎚 Mixers individuels pour chaque player
AudioMixer4              mixer1;

AudioOutputI2S audioOutput;      // Sortie audio
AudioControlSGTL5000 audioShield;

// 🎚 Assignation des connexions audio
/* AudioConnection patchCord1(player1, reverb1);
AudioConnection patchCord2(reverb1, 0, delay1, 0);
AudioConnection patchCord3(delay1, 0, mixer1, 0);

AudioConnection patchCord4(player2, reverb2);
AudioConnection patchCord5(reverb2, 0, delay2, 0);
AudioConnection patchCord6(delay2, 0, mixer2, 0);

AudioConnection patchCord7(player3, reverb3);
AudioConnection patchCord8(reverb3, 0, delay3, 0);
AudioConnection patchCord9(delay3, 0, mixer3, 0);

AudioConnection patchCord10(player4, reverb4);
AudioConnection patchCord11(reverb4, 0, delay4, 0);
AudioConnection patchCord12(delay4, 0, mixer4, 0);
 */
AudioConnection patchCord16(player1,0, mixer1, 0);
AudioConnection patchCord17(player2,0, mixer1, 1);
AudioConnection patchCord18(player3,0, mixer1, 2);
AudioConnection patchCord19(player4,0, mixer1, 3);

// 🔊 Chaque player envoie du son vers les deux canaux (stéréo)
AudioConnection patchCord13(mixer1, 0, audioOutput, 0);
AudioConnection patchCord14(mixer1, 0, audioOutput, 1);


// Potentiomètres
const int potReverb = A0;  // Contrôle la réverbération
const int potDelay = A1;   // Contrôle l'écho
const int potVolume = A2;  // Contrôle le volume principal

// 🛠️ Boutons
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
int mixage[4] = {0, 0, 0, 0};

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
        return;
    }

    // Configuration des boutons
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(stopButton, INPUT_PULLUP);
    pinMode(recordButton, INPUT_PULLUP);

    // Réglage initial des effets
/*     reverb1.reverbTime(1.5);  // Réverbération (1.5 sec)
    delay1.delay(0, 250);       */// Écho de 250 ms
    // Initialisation des mixeurs (volume à 80% au départ)
}
//  Fonction pour démarrer l'enregistrement
void startRecording() {
    isRecording = true;
    recorder.begin();
}

// Fonction pour enregistrer depuis le micro
void recordAudio(int numeropiste) {

    Serial.println("func plus tard");
    }


void resetMixage(int mixage[], int taille, int index) {
    for (int i = 0; i < taille; i++) {
        mixage[i] = (i == index) ? 1 : 0;
    }
}


// 📌 Sélection du Player actif (via un potentiomètre)
int getSelectedPlayer() {
  return selectedMemory;                  
}
void ToutesPistes(int a) {

  if (a == 5){

        player1.play("SDTEST1.wav");
        player2.play("SDTEST2.WAV");
        player3.play("SDTEST3.WAV");
        player4.play("SDTEST4.WAV");
        Serial.println("Reprends les sons.");
        delay(300);
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


void loop() {
    // Ajustement des effets avec les potentiomètres
/*     float reverbLevel = map(analogRead(potReverb), 0, 1023, 0, 3000) / 1000.0; // 0 à 3 sec
    float delayLevel = map(analogRead(potDelay), 0, 1023, 50, 500); // 50 à 500 ms
    float volumeLevel = map(analogRead(potVolume), 0, 1023, 0, 100) / 100.0; // 0 à 1.0

    reverb1.reverbTime(reverbLevel);
    delay1.delay(0, delayLevel);
    audioShield.volume(volumeLevel); */

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

    if (digitalRead(button3) == LOW && ) {
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

    if (digitalRead(button4) == LOW) {
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

        if (player4.isPlaying()){

        player1.stop();
        player2.stop();
        player3.stop();
        player4.stop();
        Serial.println("Arrêt les sons.");
        delay(300);

        }else{

        player1.play("SDTEST1.WAV");
        player2.play("SDTEST2.WAV");
        player3.play("SDTEST3.WAV");
        player4.play("SDTEST4.WAV");
        Serial.println("Reprends les sons.");
        delay(300);

        }
    }

    //  Enregistrement du micro
    if (digitalRead(recordButton) == LOW && !isRecording) {
        Serial.println("Début enregistrement...");
        startRecording();
        delay(300);
    }

    if (isRecording) {
        recordAudio(selectedMemory);
    }

/*     setReverb();  
    setDelay();    
    setVolume();*/
}




// 🎛 Appliquer la réverbération au player sélectionné
/* void setReverb() {
  int playerNumber = getSelectedPlayer();
  float reverbLevel = analogRead(potReverb) / 1023.0 * 4.0;

  switch (playerNumber) {
    case 1: reverb1.reverbTime(reverbLevel); break;
    case 2: reverb2.reverbTime(reverbLevel); break;
    case 3: reverb3.reverbTime(reverbLevel); break;
    case 4: reverb4.reverbTime(reverbLevel); break;
  }
} */

// ⏳ Appliquer l'écho (delay) au player sélectionné
/* void setDelay() {
  int playerNumber = getSelectedPlayer();
  float delayLevel = analogRead(potDelay) / 1023.0 * 500;

  switch (playerNumber) {
    case 1: delay1.delay(0, delayLevel); break;
    case 2: delay2.delay(0, delayLevel); break;
    case 3: delay3.delay(0, delayLevel); break;
    case 4: delay4.delay(0, delayLevel); break;
  }
}

// 🔊 Ajuster le volume du player sélectionné
void setVolume() {
  int playerNumber = getSelectedPlayer();
  float volumeLevel = analogRead(potVolume) / 1023.0;
 
  switch (playerNumber) {
    case 1: mixer1.gain(playerNumber - 1 , volumeLevel); break;
    case 2: mixer1.gain(playerNumber - 1, volumeLevel); break;
    case 3: mixer1.gain(playerNumber - 1, volumeLevel); break;
    case 4: mixer1.gain(playerNumber - 1, volumeLevel); break;
  }
}


 */
/*
AudioPlaySdWav playWav1; 
AudioPlaySdWav playWav2; 
AudioPlaySdWav playWav3;
AudioPlaySdWav playWav4; 
AudioMixer4 mixer; 
AudioControlSGTL5000 audioshield;
AudioOutputI2S out;

AudioConnection patchCord1(playWav1, 0, mixer, 0);
AudioConnection patchCord2(playWav2, 0, mixer, 1);
AudioConnection patchCord3(playWav3, 0, mixer, 2);
AudioConnection patchCord4(playWav4, 0, mixer, 3);
AudioConnection patchCord5(mixer, 0, out, 0);
AudioConnection patchCord6(mixer, 0, out, 1);



#define SDCARD_MOSI_PIN  11   
#define SDCARD_SCK_PIN   13  

void setup() {
    Serial.begin(9600);
    AudioMemory(80); // Allouer de la mémoire audio

    audioshield.enable();
    audioshield.volume(0.4); 

    // Initialize the SD card
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    if (!(SD.begin())) {
      
      while (1) {
        Serial.println("Unable to access the SD card");
        delay(500);
      }
    }

    // Démarrage des pistes audio
    playWav1.play("SDTEST1.WAV");
    delay(10); 
    playWav2.play("SDTEST2.WAV");
    delay(10); 
    playWav3.play("SDTEST3.WAV");
    delay(10); 
    playWav4.play("SDTEST4.WAV");
    

    mixer.gain(0, 0.4); 
    mixer.gain(1, 0.4); 
    mixer.gain(2, 0.4); 
    mixer.gain(3, 0.4); 
    
    
}

void loop() {

}*/
