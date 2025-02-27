
#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            input;           //xy=105,63
AudioRecordQueue         queue1;         //xy=281,63
AudioPlaySdRaw           playRaw1;       //xy=302,157
AudioOutputI2S           output;           //xy=470,120
AudioConnection          patchCord1(input, 0, queue1, 0);
AudioConnection          patchCord3(playRaw1, 0, output, 0);
AudioConnection          patchCord4(playRaw1, 0, output, 1);
AudioControlSGTL5000     audioShield;     //xy=265,212
// GUItool: end automatically generated code

// For a stereo recording version, see this forum thread:
// https://forum.pjrc.com/threads/46150?p=158388&viewfull=1#post158388

// A much more advanced sound recording and data logging project:
// https://github.com/WMXZ-EU/microSoundRecorder
// https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup
// https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop =   Bounce(1, 8);  // 8 = 8 ms debounce time


const int myInput = AUDIO_INPUT_MIC;


// Use these with the Teensy Audio Shield
#define SDCARD_MOSI_PIN  11   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   13  // Teensy 4 ignores this, uses pin 13


// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording

// The file where data is recorded
File frec;

void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);


  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  audioShield.micGain(10);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin())) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}


void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();

  // Respond to button presses
  if (buttonRecord.fallingEdge() && mode == 0) {
    Serial.println("Record Button Press");

    startRecording();
  }
  if (buttonStop.fallingEdge() && mode == 1) {
    Serial.println("Stop Button Press");
    stopRecording();

  }


  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }

}


void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];

    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
    
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
}

