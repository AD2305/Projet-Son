#include "Record.h"



Record::Record() :
AudioPlaySdWav * const playerlist[4] = {&player1, &player2, &player3, &player4}
{
  Serial.begin(9600);
  AudioMemory(40);

  // Initialisation du codec audio
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(10);  
  audioShield.volume(1);


  // Connexion carte SD
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin())) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

Record::~Record(){}


void Record::startRecording(int memory){
  Serial.println("startRecording");
  String file = String("RECORD") + memory + ".RAW";
  if (SD.exists(file.c_str())) {
    SD.remove(file.c_str());
  }
  frec = SD.open(file.c_str(), FILE_WRITE);
  if (frec) {
    queue1.begin();
    isRecording = true;
  }
  
}

void Record::recordAudio(void){
  if (queue1.available() >= 2) {
    byte buffer[512];

    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
  
  }
}

void Record::stopRecording(void){
  queue1.end();
    
  while (queue1.available() > 0) {
    frec.write((byte*)queue1.readBuffer(), 256);
    queue1.freeBuffer();
  }
  frec.close();   
  isRecording = false;
  Serial.println("Enregistrement terminé !");
}

void Record::button(int btn){
  Serial.println("SDTEST1.WAV");
  player1.play("SDTEST1.WAV");
  selectedMemory = 1;
  stopSounds(selectedMemory);
  Serial.println("Mémoire 1 sélectionnée");
  resetMixage(mixage,4,selectedMemory);
  mixage[selectedMemory] += 1 ;
  if (mixage[selectedMemory] % 2 == 0){
    selectedMemory = 5 ;
    toutesPistes(selectedMemory);
  }
  delay(500);
}

void Record::resetMixage(int mixage[], int taille, int index) {
  for (int i = 0; i < taille; i++) {
      mixage[i] = (i == index) ? 1 : 0;
  }
}

void Record::readingsound(int memory) {
  Serial.print("Lecture de la mémoire ");
  Serial.println(memory);
  String filename = String("RECORD") + memory + ".RAW";
  playerlist[memory-1]->play(filename.c_str());
  
}

void Record::toutesPistes(int a) {

  if (a == 5){

        player1.play("SDTEST1.wav");
        player2.play("SDTEST2.WAV");
        player3.play("SDTEST3.WAV");
        player4.play("SDTEST4.WAV");
        Serial.println("Reprends les sons.");
        delay(300);
  }

}

void Record::stopSounds(int number) {
  if (number >= 1 && number <= 4) {
      if (number != 1) player1.stop();
      if (number != 2) player2.stop();
      if (number != 3) player3.stop();
      if (number != 4) player4.stop();
  }
  Serial.print("Arrêt de tous les sons sauf la piste ");
  Serial.println(number);
}

void Record::update(void){
  int potValue = analogRead(potPin);
    float gain = map(potValue, 0, 1023, 0, 1);
    audioShield.volume(gain);
    
    //  Changer l'espace mémoire sélectionné
    if (digitalRead(button1) == LOW) {
        button(button1);
    }
    if (digitalRead(button2) == LOW) {
        button(button2);
    }
    if (digitalRead(button3) == LOW) {
        button(button3);
    }
    if (digitalRead(button4) == LOW) {
        button(button4);
    }

    IF

    // Début de l'enregistrement
    if (digitalRead(5) == LOW && !isRecording) {
        Serial.println("Enregistrement en cours...");
        startRecording(piste);
        piste=piste+1;
        delay(200);
    }

    //  Arrêter l'enregistrement après la durée définie
    if (isRecording) {
        for(auto runUntil = std::chrono::system_clock::now() + std::chrono::seconds(5);std::chrono::system_clock::now() < runUntil;)
	      {
            recordAudio(selectedMemory);
        }
        stopRecording();
    }

   
    
    readingsound(selectedMemory);
}


