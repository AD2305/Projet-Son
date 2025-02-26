#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

AudioInputI2S  input;
AudioPlaySdRaw playRaw;

AudioPlaySdRaw playRaw1;
AudioPlaySdRaw playRaw2;
AudioPlaySdRaw playRaw3;
AudioPlaySdRaw playRaw4;
AudioRecordQueue queue1;
AudioMixer4 mixer;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;


AudioConnection patchCord1(playRaw1, 0, mixer, 0);
AudioConnection patchCord2(playRaw2, 0, mixer, 1);
AudioConnection patchCord3(playRaw3, 0, mixer, 2);
AudioConnection patchCord4(playRaw4, 0, mixer, 3);
AudioConnection patchCord5(mixer, 0, out, 0);
AudioConnection patchCord6(mixer, 0, out, 1);
AudioConnection patchCord7(input, 0, queue1, 0);


int mixage[4] = {0, 0, 0, 0};
float volume[4] = {0.5, 0.5, 0.5, 0.5};
int selectedMemory=0;
String son[4]={"SDTEST1.WAV","SDTEST2.WAV","SDTEST3.WAV","SDTEST4.WAV"};
bool play=false;
int piste=0;
AudioPlaySdRaw * const playerlist[4] = {&playRaw1, &playRaw2, &playRaw3, &playRaw4};

File file;
bool isRecording=false;


void setup() {
    Serial.begin(115200);
    AudioMemory(80);
    
    // Initialisation du Shield Audio
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_MIC);
    audioShield.micGain(20); 
    audioShield.volume(0.5);
    
    // Vérification de la carte SD
    if (!SD.begin()) {
        Serial.println("Erreur : Carte SD non détectée !");
        while (1);
    }
    Serial.println(" Carte SD OK !");

}

void loop() {
  if (Serial.available() > 0) {  // Vérifie si des données sont reçues
        String data = Serial.readStringUntil('\n'); 

        //Reset
        if (data=="reset") reset();

        if (data.startsWith("PISTE:")){
            String message=data.substring(6);
    
            //  Changer l'espace mémoire sélectionné
            if (message == "1") {
                play=true;
                bouton(1);
                stop();
            }
            if (message == "2") {
                play=true;
                bouton(2);
                stop();
            }
            if (message == "3") {
                play=true;
                bouton(3);
                stop();
            }
            if (message == "4") {
                play=true;
                bouton(4);
                stop();
            }

            if (message == "stop") {
                play=false;
                stop();
                for (int i = 0; i < 4; i++) {
                    mixage[i] = 0;
                }
            }

            if (message == "start") {
                play=true;
                selectedMemory=5;
                stop();
            }

            if (message=="record"){
              play=false;
              startRecording(piste);
              piste=piste+1;
            }

            if (message=="stopRecord"){
              stopRecording();
            }
        }

        if (data.startsWith("VOLUME:")){
          int value=data.substring(7).toInt();
          setVolume(value);
        }

        

  }
  if (isRecording) {
    recordAudio();
  }else if (play==true) {
    readingsound(selectedMemory);
  }
  
  
}


void reset(){
  piste=0;
  stop();
  if (isRecording) stopRecording();
  for (int i=0 ; i<4; i++){
    String filename = String("RECORD") + i + ".RAW";
    if (SD.exists(filename.c_str())) {
      SD.remove(filename.c_str());
    }
  }

}



void resetMixage( int index) {
    for (int i = 0; i < 4; i++) {
        if (i!=index) mixage[i] =  0;
    }
}

void bouton(int btn){
  selectedMemory=btn-1;
  if (mixage[selectedMemory] == 1){
    selectedMemory=5;
  }else {
    resetMixage(selectedMemory);
    mixage[selectedMemory] = 1 ;
  }
}

void toutesPistes(){
    int i=0;
    while (i<piste){
      mixer.gain(i,volume[i]);
      String filename = String("RECORD") + i + ".RAW";
      if (!playerlist[i]->isPlaying()) playerlist[i]->play(filename.c_str());
      delay(10);
      i+=1;
    }
    for (int i = 0; i < 4; i++) {
        mixage[i] = 0;
    }
}

void stop(){
  playRaw1.stop();
  playRaw2.stop();
  playRaw3.stop();
  playRaw4.stop();
}

void readingsound(int memory) {
  if (memory!=5){
    String filename = String("RECORD") + memory + ".RAW"; 
    //String filename =son[memory];
    if (!playRaw1.isPlaying()){
      playRaw1.play(filename.c_str());
    }
    mixer.gain(0,volume[memory]);
    mixer.gain(1,0);
    mixer.gain(2,0);
    mixer.gain(3,0);
  }
  else{
    toutesPistes();
  }  
}


//Recording
void startRecording(int memory){
  Serial.println("startRecording");
  String filename = String("RECORD") + memory + ".RAW";
  if (SD.exists(filename.c_str())) {
    SD.remove(filename.c_str());
  }
  file = SD.open(filename.c_str(), FILE_WRITE);
  if (file) {
    queue1.begin();
    isRecording = true;
  }
}

void recordAudio(){
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    file.write(buffer, 512);
  
  }
}

void stopRecording(){
  queue1.end();
  while (queue1.available() > 0) {
    file.write((byte*)queue1.readBuffer(), 256);
    queue1.freeBuffer();
  }
  file.close();   
  isRecording = false;
  Serial.println("Enregistrement terminé !");
}


//Volume et effets
void setVolume(int volumeLevel) {
  float volumeValue = volumeLevel / 100.0;
  volume[selectedMemory]=(volumeValue*volumeValue);
  mixer.gain(0, volumeValue);
}
