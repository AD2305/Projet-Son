#ifndef RECORD_H
#define RECORD_H

#include "Init_Audio_Sys.h"
#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

class Record : public AudioStream
{
    public:
        Record();
        ~Record();

        //function
        void startRecording(int memory);
        void recordAudio(void);
        void stopRecording(void);
        void button(int btn);
        void readingsound(int memory);
        void resetMixage(int mixage[], int taille, int index);
        virtual void update(void);

    
    private:
        //variable
        const int button1, button2, button3, button4;
        bool isRecording;
        File file;
        int piste,selectedMemory, potPin;
        Bounce recordButton, playButton;
}

#endif