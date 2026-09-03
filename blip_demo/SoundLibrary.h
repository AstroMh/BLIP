#ifndef SOUND_LIBRARY_H
#define SOUND_LIBRARY_H

#include <Arduino.h>

struct SoundEffect {
  const char* name;
  int frequencies[16];
  int durations[16];
  int length;
  int repeatCount;
};

class SoundPlayer {
  private:
    static const int MAX_SOUNDS = 10;
    SoundEffect* soundLibrary[MAX_SOUNDS];
    int soundCount;
    
    int currentSoundIndex;
    int currentNote;
    int currentRepeat;
    unsigned long lastNoteTime;
    
    bool soundIsPlaying;
    bool soundHasStarted;
    bool soundIsFinished;
    int buzzerPin;
    
  public:
    SoundPlayer();
    void begin(int pin);
    void addSound(SoundEffect* sound);
    void playRandomSound(unsigned long currentTime);
    void playSoundByIndex(int index, unsigned long currentTime);
    void stop();
    bool isPlaying();
    bool isFinished();
    void reset();
    int getSoundCount();
    const char* getSoundName(int index);
};

extern SoundEffect soundWhiny;
extern SoundEffect soundBeeping;
extern SoundEffect soundSiren;
extern SoundEffect soundRobotTalk;
extern SoundEffect soundGlitchy;
extern SoundEffect soundAlarm;
extern SoundEffect soundComplaint;
extern SoundEffect soundNagging;

#endif 