#include "SoundLibrary.h"


SoundEffect soundHappyChirp = {
  "Happy Chirp",
  {523, 659, 784, 1046, 784, 659, 523, 659, 784, 1046, 1318, 1046, 784, 659, 523, 0},
  {80, 80, 80, 120, 80, 80, 80, 80, 80, 120, 160, 120, 80, 80, 160, 40},
  16, 2
};

SoundEffect soundFairyTale = {
  "Fairy Tale Sparkle",
  {784, 988, 1175, 1568, 1175, 988, 784, 988, 1175, 1568, 1976, 1568, 1175, 988, 784, 0},
  {60, 60, 60, 140, 60, 60, 60, 60, 60, 140, 200, 140, 60, 60, 220, 30},
  16, 2
};

SoundEffect soundR2D2Happy = {
  "R2D2 Happy",
  {1200, 1800, 1400, 2200, 1600, 2400, 2000, 2600, 1800, 2200, 1600, 2000, 2400, 1800, 2000, 0},
  {40, 30, 40, 30, 50, 40, 40, 50, 40, 40, 50, 40, 40, 50, 80, 20},
  16, 3
};

SoundEffect soundR2D2Curious = {
  "R2D2 Curious",
  {800, 1600, 900, 1800, 1000, 2000, 1200, 2200, 1400, 1800, 1600, 2000, 1800, 2200, 2000, 0},
  {60, 40, 60, 40, 70, 50, 70, 50, 60, 40, 60, 40, 60, 50, 100, 20},
  16, 2
};

SoundEffect soundGiggle = {
  "Giggle",
  {880, 988, 880, 988, 880, 988, 880, 988, 1046, 988, 880, 784, 880, 988, 1046, 0},
  {50, 40, 50, 40, 50, 40, 50, 40, 80, 60, 60, 60, 50, 40, 120, 30},
  16, 2
};

SoundEffect soundMagical = {
  "Magical Twinkle",
  {1046, 1318, 1568, 2093, 1568, 1318, 1046, 1318, 1568, 2093, 2637, 2093, 1568, 1318, 1046, 0},
  {60, 60, 60, 150, 60, 60, 60, 60, 60, 150, 220, 150, 60, 60, 200, 30},
  16, 1
};

SoundEffect soundJoyful = {
  "Joyful Bounce",
  {659, 784, 988, 784, 988, 1175, 988, 1175, 1318, 1175, 1318, 1568, 1318, 1175, 988, 784},
  {70, 70, 100, 70, 70, 100, 70, 70, 100, 70, 70, 140, 70, 70, 100, 160},
  16, 2
};


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


  addSound(&soundHappyChirp);
  addSound(&soundFairyTale);
  addSound(&soundR2D2Happy);
  addSound(&soundR2D2Curious);
  addSound(&soundGiggle);
  addSound(&soundMagical);
  addSound(&soundJoyful);
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