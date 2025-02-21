#include "Init_Audio_Sys.h"
#include "Record.h"

Record recorder;

void setup() {
    // La configuration est déjà faite dans le constructeur
}

void loop() {
    recorder.update();  // Gère les boutons et l'enregistrement/lecture
}
