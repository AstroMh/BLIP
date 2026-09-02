#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "esp_random.h"
#include "bootloader_random.h" 
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define TOUCH_PIN 2
#define BUZZER_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

unsigned long lastBlinkTime = 0;
unsigned long lastLookTime = 0;
unsigned long touchStartTime = 0;

uint32_t blinkInterval = 2000;
uint32_t lookInterval = 5000;

bool isTouched = false;
bool lastTouchState = false;
bool isAnnoyed = false;

void setup() {
  Serial.begin(115200);
  bootloader_random_enable();

  pinMode(TOUCH_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed!"));
    for(;;);
  }

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setPosition(DEFAULT);
  roboEyes.setMood(DEFAULT);
  roboEyes.open();

  lastBlinkTime = millis();
  lastLookTime = millis();
  
  Serial.println("Fast Touch Test");
}

void loop() {
  unsigned long currentTime = millis(); 
  
  bool currentlyTouched = (digitalRead(TOUCH_PIN) == LOW);
  
  if (currentlyTouched && !lastTouchState) {
    roboEyes.setMood(ANGRY);
    isTouched = true;
    isAnnoyed = true;
    touchStartTime = currentTime;
  }
  else if (!currentlyTouched && lastTouchState) {
    roboEyes.setMood(DEFAULT);
    isTouched = false;
    isAnnoyed = false;
  }
  else if (currentlyTouched && isTouched && (currentTime - touchStartTime > 3000)) {
    roboEyes.setMood(DEFAULT);
    isTouched = false;
    isAnnoyed = false;
  }
  
  lastTouchState = currentlyTouched;

  if (isAnnoyed) {
    playRandomBuzzer(currentTime);
  } else {
    noTone(BUZZER_PIN);
  }

  roboEyes.update();
  blinkRandomly();
  lookRandomly();
  
  delay(1);  
}

uint32_t generateRandomNumber(uint32_t min, uint32_t max) {
  if (min >= max) return min;

  uint32_t range = max - min + 1;
  uint32_t limit = UINT32_MAX - (UINT32_MAX % range);
  uint32_t random_num;

  do {
    random_num = esp_random(); 
  } while (random_num >= limit);

  return min + (random_num % range);
}

void blinkRandomly() {
  if (millis() - lastBlinkTime >= blinkInterval) {
    roboEyes.blink();
    lastBlinkTime = millis();
    blinkInterval = generateRandomNumber(500, 7000);
  }
}

void lookRandomly() {
  if (millis() - lastLookTime >= lookInterval) {
    int direction = random(4);

    if (direction == 0) {
      roboEyes.setPosition(W);
    } else if (direction == 1) {
      roboEyes.setPosition(E);
    } else if (direction == 2){
      roboEyes.setPosition(S);
    } else {
      roboEyes.setPosition(DEFAULT);
    }

    lastLookTime = millis();
    lookInterval = generateRandomNumber(500, 10000);
  }
}


// =========== BUZZER =============

void playRandomBuzzer(unsigned long currentTime) {
  int function = random(3);

  if (function == 0) {
    playAnnoyedBuzzer1(currentTime);
  }
}

void playAnnoyedBuzzer1(unsigned long currentTime) {
  static int complaintPhase = 0;
  static unsigned long lastPhaseTime = 0;
  
  int complainSpeed = 100;
  
  if (currentTime - lastPhaseTime >= complainSpeed) {
    complaintPhase = (complaintPhase + 1) % 12;
    
    // Random "complaint" sounds
    switch(complaintPhase) {
      case 0: tone(BUZZER_PIN, 300); break;
      case 1: tone(BUZZER_PIN, 400); break;
      case 2: tone(BUZZER_PIN, 350); break;
      case 3: tone(BUZZER_PIN, 600); break;
      case 4: tone(BUZZER_PIN, 450); break;
      case 5: noTone(BUZZER_PIN); break;
      case 6: tone(BUZZER_PIN, 500); break;
      case 7: tone(BUZZER_PIN, 700); break;
      case 8: tone(BUZZER_PIN, 400); break;
      case 9: tone(BUZZER_PIN, 550); break;
      case 10: tone(BUZZER_PIN, 650); break;
      case 11: noTone(BUZZER_PIN); break;
    }
    
    lastPhaseTime = currentTime;
  }
}

void playAnnoyedBuzzer2(unsigned long currentTime) {
  static unsigned long lastNoiseTime = 0;
  
  if (currentTime - lastNoiseTime >= 30) {  // Very fast!
    int randomFreq = random(200, 1200);
    int randomDuration = random(20, 80);
    
    tone(BUZZER_PIN, randomFreq);
    lastNoiseTime = currentTime;
    
    // Random pauses to sound glitchy
    if (random(5) == 0) {
      noTone(BUZZER_PIN);
    }
  }
}

void playAnnoyedBuzzer3(unsigned long currentTime) {
  static bool beepOn = false;
  static unsigned long lastToggle = 0;
  
  int beepSpeed = 80;  // Fast beeping
  
  if (currentTime - lastToggle >= beepSpeed) {
    beepOn = !beepOn;
    if (beepOn) {
      // Random high-pitch whine
      tone(BUZZER_PIN, random(400, 900));
    } else {
      noTone(BUZZER_PIN);
    }
    lastToggle = currentTime;
  }
}
