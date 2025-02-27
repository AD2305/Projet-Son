#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
short delayline[FLANGE_DELAY_LENGTH];

#define GRANULAR_MEMORY_SIZE 12800  // Mémoire pour l'effet granular
int16_t granularMemory[GRANULAR_MEMORY_SIZE];

#define CHORUS_DELAY_LENGTH (100 * AUDIO_BLOCK_SAMPLES)
short delayline2[CHORUS_DELAY_LENGTH];

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

AudioEffectFlange flanger1, flanger2, flanger3, flanger4;
AudioEffectGranular granular1, granular2, granular3, granular4;
AudioEffectChorus chorus1, chorus2, chorus3, chorus4;


AudioConnection patchCord1(playRaw1, 0, flanger1, 0);
AudioConnection patchCord2(flanger1, 0, granular1, 0);
AudioConnection patchCord3(granular1, 0, chorus1, 0);
AudioConnection patchCord4(chorus1, 0, mixer, 0);

AudioConnection patchCord5(playRaw2, 0, flanger2, 0);
AudioConnection patchCord6(flanger2, 0, granular2, 0);
AudioConnection patchCord7(granular2, 0, chorus2, 0);
AudioConnection patchCord8(chorus2, 0, mixer, 1);

AudioConnection patchCord9(playRaw3, 0, flanger3, 0);
AudioConnection patchCord10(flanger3, 0, granular3, 0);
AudioConnection patchCord11(granular3, 0, chorus3, 0);
AudioConnection patchCord12(chorus3, 0, mixer, 2);

AudioConnection patchCord13(playRaw4, 0, flanger4,0);
AudioConnection patchCord14(flanger4, 0, granular4, 0);
AudioConnection patchCord15(granular4, 0, chorus4, 0);
AudioConnection patchCord16(chorus4, 0, mixer, 3);

AudioConnection patchCord17(mixer, 0, out, 0);
AudioConnection patchCord18(mixer, 0, out, 1);
AudioConnection patchCord19(input, 0, queue1, 0);


int mixage[4] = {0, 0, 0, 0};
float volume[4] = {0.5, 0.5, 0.5, 0.5};
float flanger[4][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
float chorus[4] = {0,0,0,0};
float granular[4] = {0,0,0,0};


//String son[4]={"SDTEST1.WAV","SDTEST2.WAV","SDTEST3.WAV","SDTEST4.WAV"};
File file;
bool isRecording=false;
bool play=false;
int piste=0;
int selectedMemory=0;

AudioPlaySdRaw * const playerlist[4] = {&playRaw1, &playRaw2, &playRaw3, &playRaw4};
AudioEffectFlange * const flangelist[4] ={&flanger1,&flanger2,&flanger3,&flanger4};
AudioEffectGranular * const granularlist[4] ={&granular1,&granular2,&granular3,&granular4};
AudioEffectChorus * const choruslist[4] ={&chorus1,&chorus2,&chorus3,&chorus4};

int s_idx = FLANGE_DELAY_LENGTH/4;
int s_depth = FLANGE_DELAY_LENGTH/4;
double s_freq = .5;



void setup() {
    Serial.begin(115200);
    AudioMemory(150);
    
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

        if (data.startsWith("FLANGER:")){
          int value=data.substring(8).toInt();
          setFlanger(value);
        }

        if (data.startsWith("CHORUS:")){
          int value=data.substring(7).toInt();
          setChorus(value);
        }

        if (data.startsWith("GRANULAR:")){
          int value=data.substring(9).toInt();
          setGranular(value);
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

void setFlanger(int value) {
    float offset = map(value, 0, 1000, 1, 100);
    float depth = map(value, 0, 1000, 1, 100);
    float delay = map(value, 0, 1000, 1, 100);
    flanger[selectedMemory][0] = offset;
    flanger[selectedMemory][1] = depth;
    flanger[selectedMemory][2] = delay;
    flangelist[selectedMemory]->voices(offset, depth, delay);
    
}

void setChorus(int value) {
    float voices = map(value, 0, 1000, 1, 10) ;
    chorus[selectedMemory]=voices;
    choruslist[selectedMemory]->voices(voices);
}

void setGranular(int value) {
    float speed = map(value, 0, 1000, 0.125, 8);
    granular[selectedMemory]=speed;
    granularlist[selectedMemory]->setSpeed(speed);
}

