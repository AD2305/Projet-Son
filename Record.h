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
        void toutesPistes(int a);
        void stopSounds(int number);
        virtual void update(void);

    
    private:
        //variable
        bool isRecording;
        File file;
        int piste=1,selectedMemory=0;
        int mixage[4]={0,0,0,0};
}

#endif