/*
Arduino drum machine, AKA "who needs friends when you've got a computer"
It has some presets (set by setPreset()), which you can check in the enum Preset. 
For the wiring, I recommend a LPF before the jack (a 103 cap to ground and a 100k pot between pin 11 of the arduino
and the cap/output) (moving knobs around is fun). 
Have fun and good luck if you have to touch this code (I'd help you, but I'm not good at this)!
*/

// Libraries
#define MOZZI_CONTROL_RATE 64
#include <Mozzi.h>
#include <Oscil.h>
#include <ADSR.h>
#include <Sample.h>
#include <samples/drums/hh_sample.h>
#include <samples/drums/snare_sample.h>
#include <samples/drums/snare_noise_sample.h>
#include <samples/drums/kick_sample.h>


Sample <HH_SAMPLE_NUM_CELLS, MOZZI_AUDIO_RATE> hatSound(HH_SAMPLE_DATA);
Sample <HH_SAMPLE_NUM_CELLS, MOZZI_AUDIO_RATE> snareSound(SNARE_SAMPLE_DATA);
Sample <HH_SAMPLE_NUM_CELLS, MOZZI_AUDIO_RATE> snareNoiseSound(SNARE_NOISE_SAMPLE_DATA);
Sample <HH_SAMPLE_NUM_CELLS, MOZZI_AUDIO_RATE> kickSound(KICK_SAMPLE_DATA);



unsigned long lastTrigger = 0;
unsigned long triggerInterval = 95000; // microseconds between each step

int stepIndex = 0;
int steps = 16; // beats per bar

//int hatPattern[16]  =  {0, 0, 1, 0,  0, 0, 1, 0,  0, 0, 0, 0,  0, 0, 1, 1}; // [NO] pattern
//int snarePattern[16] = {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 1, 0,  0, 0, 0, 0};
//int kickPattern[16] =  {0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0};

int hatPattern[64]  =  {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
int snarePattern[64] = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0 ,0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
int kickPattern[64] =  {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};

// PRESETS: 
int hatPre1[16]  =  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}; // random shit I made
int snarePre1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,1, 0, 0, 0, 0};
int kickPre1[16] =  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1};
int stepsPre1 = 16;

int hatHouse[16]  =  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1}; // house / punchinpunchin, tomato, tomato
int snareHouse[16] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};
int kickHouse[16] =  {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}; // it would sound better with another kick though
int stepsHouse = 16;

int hatRock1[16]  =  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 ,0, 1, 0, 1, 0}; // rock beat, money beat
int snareRock1[16] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
int kickRock1[16] =  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ,0, 0, 0, 0, 0};
int stepsRock1 = 16;

int hatFunk1[16]  =  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
int snareFunk1[16] = {0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0};
int kickFunk1[16] =  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0};
int stepsFunk1 = 16;

int hatAmen[64]  =  {1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,     1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,   1, 0, 1, 0,     1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,   1, 0, 1, 0,     1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0};
int snareAmen[64] = {0, 0, 0, 0,  1, 0, 0, 1,  0, 1, 0 ,0,  1, 0, 0, 1,     0, 0, 0, 0,  1, 0, 0, 1,  0, 1, 0 ,0,   1, 0, 0, 1,     0, 0, 0, 0,  1, 0, 0, 1,  0, 1, 0 ,0,   0, 0, 1, 0,     0, 1, 0, 0,  1, 0, 0, 1,  0, 1, 0, 0,  0, 0, 1, 0};
int kickAmen[64] =  {1, 0, 1, 0,  0, 0, 0, 0,  0, 0, 1, 1,  0, 0, 0, 0,     1, 0, 1, 0,  0, 0, 0, 0,  0, 0, 1, 1,   0, 0, 0, 0,     1, 0, 1, 0,  0, 0, 0, 0,  0, 0, 1, 0,   0, 0, 0, 0,     0, 0, 1, 1,  0, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 0};
int stepsAmen = 64;

      enum Preset {NO, PRE1, HOUSE, ROCK1, FUNK1, AMEN}; // pretty pretty enumerator
      
      void setPreset(Preset pre)
      {
        switch(pre) 
        {
          case NO: break;
          
          case PRE1:
          {
            memcpy(hatPattern,   hatPre1,   sizeof(hatPre1)); // assign the pre1 pattern as the one being used
            memcpy(snarePattern, snarePre1, sizeof(snarePre1));
            memcpy(kickPattern,  kickPre1,  sizeof(kickPre1));
            steps = stepsPre1;
            break;
          }
          case HOUSE: 
          {
            memcpy(hatPattern,   hatHouse,   sizeof(hatHouse));
            memcpy(snarePattern, snareHouse, sizeof(snareHouse));
            memcpy(kickPattern,  kickHouse,  sizeof(kickHouse));
            steps = stepsHouse;
            break;
          }
          
          case ROCK1: 
          {
            memcpy(hatPattern,   hatRock1,   sizeof(hatRock1));
            memcpy(snarePattern, snareRock1, sizeof(snareRock1));
            memcpy(kickPattern,  kickRock1,  sizeof(kickRock1));
            steps = stepsRock1;
            break;   
          }
          case FUNK1:
          {
            memcpy(hatPattern,   hatFunk1,   sizeof(hatFunk1));
            memcpy(snarePattern, snareFunk1, sizeof(snareFunk1));   
            memcpy(kickPattern,  kickFunk1,  sizeof(kickFunk1));  
            steps = stepsFunk1;       
          break; 
          }
          case AMEN:
          {
            memcpy(hatPattern,   hatAmen,   sizeof(hatAmen));
            memcpy(snarePattern, snareAmen, sizeof(snareAmen));   
            memcpy(kickPattern,  kickAmen,  sizeof(kickAmen));  
            steps = stepsAmen;       
          break; 
          }
        }
      }

unsigned long swing = 5000; //between 0 and triggerInterval 
int hatFreq = 20;
int snareFreq = 20;
int kickFreq = 20;

void setup(){
  
  setPreset(AMEN); // set a preset

  startMozzi(); // get this shit started
    hatSound.setFreq(hatFreq);
    snareSound.setFreq(snareFreq);
    snareNoiseSound.setFreq(snareFreq);
    kickSound.setFreq(kickFreq);
}

// EVENTS (one for each instrument)
void triggerHat() {
  hatSound.start();
}
void triggerKick()
{
  kickSound.start();
}
void triggerSnare()
{
  snareSound.start();
  snareNoiseSound.start();
}

void step() 
{

}

void updateControl(){
  
unsigned long interval = triggerInterval;
if (stepIndex % 2 != 0) {
  interval += swing;  // delay odd steps
}

if (mozziMicros() - lastTrigger >= interval) {
  lastTrigger = mozziMicros();

  if (hatPattern[stepIndex])  { triggerHat(); } 
  if (kickPattern[stepIndex]) { triggerKick(); }
  if (snarePattern[stepIndex]) { triggerSnare(); }

  stepIndex = (stepIndex + 1) % steps;
}

}


AudioOutput updateAudio(){
   int8_t hat = 0;
   int8_t snare = 0;
   int8_t noise = 0;
   int8_t kick = 0;
    
   if (hatSound.isPlaying()) {
     hat = hatSound.next();
    }
   if (snareSound.isPlaying()) {
     snare = snareSound.next();
    }
    if (snareNoiseSound.isPlaying()) {
     noise = snareNoiseSound.next();
    }
   if (kickSound.isPlaying()) {
     kick = kickSound.next();
    }
    
  int8_t sound = (snare + hat + kick+noise)/4; // thanos' gauntlet

  return MonoOutput(sound);
}

void loop(){
  audioHook();
}
// That's it!
