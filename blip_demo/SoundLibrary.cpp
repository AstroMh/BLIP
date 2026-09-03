#include "SoundLibrary.h"


SoundEffect soundWhiny = {
  "Whiny Complaint",
  {400, 450, 400, 500, 400, 550, 400, 600, 400, 550, 400, 500},
  {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80},
  12, 2
};

SoundEffect soundBeeping = {
  "Rapid Beeping",
  {600, 400, 600, 400, 600, 400, 800, 400, 600, 400, 600, 400},
  {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50},
  12, 3
};

SoundEffect soundSiren = {
  "Siren",
  {300, 330, 360, 390, 420, 450, 480, 510, 540, 570, 600, 570, 540, 510, 480, 450},
  {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60},
  16, 1
};

SoundEffect soundRobotTalk = {
  "Robot Talk",
  {300, 0, 450, 0, 600, 0, 350, 0, 500, 0, 400, 0, 550, 0, 650, 0},
  {40, 20, 40, 20, 40, 20, 40, 20, 40, 20, 40, 20, 40, 20, 40, 20},
  16, 2
};

SoundEffect soundGlitchy = {
  "Glitchy Static",
  {200, 800, 150, 900, 300, 700, 250, 850, 100, 950, 400, 600},
  {30, 20, 40, 20, 30, 20, 40, 20, 30, 20, 40, 20},
  12, 3
};

SoundEffect soundAlarm = {
  "Alarm",
  {500, 500, 500, 500, 700, 700, 700, 700, 500, 500, 500, 500, 300, 300, 300, 300},
  {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  16, 2
};

SoundEffect soundComplaint = {
  "Complaint",
  {500, 450, 400, 350, 300, 350, 400, 450, 500, 450, 400, 350},
  {100, 100, 100, 100, 200, 100, 100, 100, 100, 100, 100, 200},
  12, 1
};

SoundEffect soundNagging = {
  "Nagging",
  {400, 500, 400, 600, 400, 500, 400, 700, 400, 500, 400, 600, 400, 500, 400, 800},
  {60, 40, 60, 40, 60, 40, 60, 40, 60, 40, 60, 40, 60, 40, 60, 40},
  16, 2
};


SoundPlayer::SoundPlayer() {
  soundCount = 0;
  currentSoundIndex = -1;
  currentNote = 0;
  currentRepeat = 0;
  lastNoteTime = 0;
  soundIsPlaying = false;
  soundHasStarted = false;
  soundIsFinished = true;
  buzzerPin = 4;
}

void SoundPlayer::begin(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
  
  addSound(&soundWhiny);
  addSound(&soundBeeping);
  addSound(&soundSiren);
  addSound(&soundRobotTalk);
  addSound(&soundGlitchy);
  addSound(&soundAlarm);
  addSound(&soundComplaint);
  addSound(&soundNagging);
}

void SoundPlayer::addSound(SoundEffect* sound) {
  if (soundCount < MAX_SOUNDS) {
    soundLibrary[soundCount] = sound;
    soundCount++;
  }
}

void SoundPlayer::playRandomSound(unsigned long currentTime) {
  if (soundIsPlaying) {
    playSoundByIndex(currentSoundIndex, currentTime);
    return;
  }
  
  if (soundIsFinished || !soundHasStarted) {
    currentSoundIndex = random(soundCount);
    soundIsFinished = false;
    soundIsPlaying = false;
    soundHasStarted = false;
    
    Serial.print("🔊 Playing: ");
    Serial.println(soundLibrary[currentSoundIndex]->name);
  }
  
  playSoundByIndex(currentSoundIndex, currentTime);
}

void SoundPlayer::playSoundByIndex(int index, unsigned long currentTime) {
  if (index < 0 || index >= soundCount) return;
  
  SoundEffect* sound = soundLibrary[index];
  
  if (!soundHasStarted) {
    currentNote = 0;
    currentRepeat = 0;
    lastNoteTime = currentTime;
    soundHasStarted = true;
    soundIsPlaying = true;
    soundIsFinished = false;
    return;
  }
  
  if (currentTime - lastNoteTime >= sound->durations[currentNote]) {
    noTone(buzzerPin);
    currentNote++;
    
    if (currentNote >= sound->length) {
      currentRepeat++;
      if (currentRepeat >= sound->repeatCount) {
        soundIsPlaying = false;
        soundIsFinished = true;
        soundHasStarted = false;
        return;
      }
      currentNote = 0;
    }
    
    if (sound->frequencies[currentNote] > 0) {
      tone(buzzerPin, sound->frequencies[currentNote]);
    }
    
    lastNoteTime = currentTime;
  }
}

void SoundPlayer::stop() {
  noTone(buzzerPin);
  soundIsPlaying = false;
  soundIsFinished = true;
  soundHasStarted = false;
}

bool SoundPlayer::isPlaying() {
  return soundIsPlaying;
}

bool SoundPlayer::isFinished() {
  return soundIsFinished;
}

void SoundPlayer::reset() {
  stop();
  currentSoundIndex = -1;
  currentNote = 0;
  currentRepeat = 0;
  lastNoteTime = 0;
}

int SoundPlayer::getSoundCount() {
  return soundCount;
}

const char* SoundPlayer::getSoundName(int index) {
  if (index < 0 || index >= soundCount) return "Unknown";
  return soundLibrary[index]->name;
}