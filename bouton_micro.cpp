#include <Audio.h>

// Déclaration des objets audio
AudioInputI2S in;  
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioRecordQueue recorder;
AudioPlayMemory player;
AudioConnection patchCord0(in, 0, recorder, 0);
AudioConnection patchCord1(in, 0, out, 0);
AudioConnection patchCord2(in, 0, out, 1);
AudioConnection patchCord3(player, 0, out, 0);
AudioConnection patchCord4(player, 0, out, 1);

// Définition des boutons et du potentiomètre
const int button1 = 1;  // Sélection mémoire 1
const int button2 = 2;  // Sélection mémoire 2
const int button3 = 3;  // Sélection mémoire 3
const int button4 = 4;  // Sélection mémoire 4
const int recordButton = 0;  // Bouton d'enregistrement
const int playButton = 5;  // Bouton de lecture
const int potPin = A0;  // Potentiomètre pour régler le gain

// Variables de contrôle
int selectedMemory = 0;  // Espace mémoire choisi (0 à 4)
bool isRecording = false;
int mixage[4] = {0, 0, 0, 0};
const int recordDuration = 5;  // Durée d'enregistrement en secondes
const int sampleRate = 44100;  // Fréquence d'échantillonnage

// Tableau de buffers pour stocker plusieurs enregistrements

void setup() {
    Serial.begin(9600);
    AudioMemory(10);
    
    // Initialisation du codec audio
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_MIC);
    audioShield.micGain(10);  // Gain par défaut
    audioShield.volume(1);

    // Configuration des boutons
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(recordButton, INPUT_PULLUP);
}

void loop() {
    //  Ajuster le gain du micro avec le potentiomètre
    int potValue = analogRead(potPin);
    float gain = map(potValue, 0, 1023, 0, 1);  // Gain entre 0 et 1 
    audioShield.volume(gain);
    
    //  Changer l'espace mémoire sélectionné
    if (digitalRead(button1) == LOW) {
        selectedMemory = 1;
        Serial.println("Mémoire 1 sélectionnée");
        delay(200);
        resetMixage(mixage,sizeof(mixage),selectedMemory);
        mixage[1] += 1 ;
        if (mixage[1] % 2 == 0){
            selectedMemory = 5
        }
    }
    if (digitalRead(button2) == LOW) {
        selectedMemory = 2;
        Serial.println("Mémoire 2 sélectionnée");
        delay(200);
        mixage[2] += 1 ;
        resetMixage(mixage,sizeof(mixage),selectedMemory);
        if (mixage[2] % 2 == 0){
            selectedMemory = 5
        }
    }
    if (digitalRead(button3) == LOW) {
        selectedMemory = 3;
        Serial.println("Mémoire 3 sélectionnée");
        delay(200);
        resetMixage(mixage,sizeof(mixage),selectedMemory);
        mixage[3] += 1 ;
        if (mixage[3] % 2 == 0){
            selectedMemory = 5
        }
    }
    if (digitalRead(button4) == LOW) {
        selectedMemory = 4;
        Serial.println("Mémoire 4 sélectionnée");
        delay(200);
        resetMixage(mixage,sizeof(mixage),selectedMemory);
        mixage[4] += 1;
        if (mixage[4] % 2 == 0){
            selectedMemory = 5
        }
    }

    // Début de l'enregistrement
    if (digitalRead(recordButton) == LOW && !isRecording) {
        Serial.println("Enregistrement en cours...");
        startRecording();
        delay(200);
    }

    //  Arrêter l'enregistrement après la durée définie
    if (isRecording) {
        recordAudio(selectedMemory);
    }

   
    
    readingsound(selectedMemory);

    }
}

void resetMixage(int mixage[], int taille, int index) {
    for (int i = 0; i < taille; i++) {
        mixage[i] = (i == index) ? 1 : 0;
    }
}


void presse2fois(){
    
}
// Fonction pour démarrer l'enregistrement
void startRecording() {
    isRecording = true;
}

// Fonction pour stocker l'enregistrement dans l'espace mémoire sélectionné
void recordAudio(int memory) {

    //Fonction anais
    


    // Arrêter l'enregistrement après la durée prévue
    
        recorder.end();
        isRecording = false;
        Serial.println("Enregistrement terminé !");
}

// Fonction pour lire un enregistrement stocké
void readingsound(int memory) {
    Serial.print("Lecture de la mémoire ");
    Serial.println(memory);

    //fonction anais
}
