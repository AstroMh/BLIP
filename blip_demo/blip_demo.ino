#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "esp_random.h"
#include "bootloader_random.h" 
#include <FluxGarage_RoboEyes.h>
#include "SoundLibrary.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define TOUCH_PIN 2
#define BUZZER_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);
SoundPlayer soundPlayer;

unsigned long lastBlinkTime = 0;
unsigned long lastLookTime = 0;
unsigned long lastDebounceTime = 0;
unsigned long touchStartTime = 0; 

uint32_t blinkInterval = 2000;
uint32_t lookInterval = 5000;


bool isAngry = false;           
bool lastTouchState = false;    
bool soundTriggered = false;    
bool currentTouchState = false;
bool soundPlayed = false;      

const int debounceDelay = 50;
const unsigned long HOLD_TIME = 3000;  

void setup() {
  Serial.begin(115200);
  bootloader_random_enable();

  pinMode(TOUCH_PIN, INPUT_PULLUP);
  soundPlayer.begin(BUZZER_PIN);

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
}

void loop() {
  unsigned long currentTime = millis();  
  
  readTouch(currentTime);
  
  updateRobotState(currentTime);
  
  roboEyes.update();
  handleBlink(currentTime);
  handleLook(currentTime);
  
  handleSound(currentTime);
  
  delay(5);
}


void readTouch(unsigned long currentTime) {
  bool rawTouchState = (digitalRead(TOUCH_PIN) == HIGH); 
  
  if (rawTouchState != lastTouchState) {
    lastDebounceTime = currentTime;
  }
  
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    currentTouchState = rawTouchState;
  }
  
  lastTouchState = rawTouchState;
}


void updateRobotState(unsigned long currentTime) {
  static bool previousState = false;
  
  if (currentTouchState == true && previousState == false) {
    touchStartTime = currentTime;
    soundPlayed = false;
  }
  
  if (currentTouchState == false && previousState == true) {
    roboEyes.setMood(DEFAULT);
    isAngry = false;
    soundPlayer.stop();
    soundTriggered = false;
    soundPlayed = false;
  }
  
  if (currentTouchState == true && !isAngry) {
    roboEyes.setMood(ANGRY);
    if ((currentTime - touchStartTime) >= HOLD_TIME) {
      isAngry = true;
      soundTriggered = false;
    } else {
      static unsigned long lastProgressTime = 0;
      if (currentTime - lastProgressTime > 500) {  // Show every 0.5 seconds
        float progress = ((float)(currentTime - touchStartTime) / HOLD_TIME) * 100;
        Serial.print("⏳ Holding: ");
        Serial.print(progress, 0);
        Serial.println("%");
        lastProgressTime = currentTime;
      }
    }
  }
  
  previousState = currentTouchState;
}

void handleSound(unsigned long currentTime) {
  if (isAngry) {
    if (!soundTriggered) {
      soundPlayer.reset();
      soundPlayer.playRandomSound(currentTime);
      soundTriggered = true;
      soundPlayed = true;
    }
    
    if (soundPlayer.isPlaying()) {
      soundPlayer.playRandomSound(currentTime);
    }
  } else {
    soundPlayer.stop();
    soundTriggered = false;
  }
}

void handleBlink(unsigned long currentTime) {
  if (currentTime - lastBlinkTime >= blinkInterval) {
    roboEyes.blink();
    lastBlinkTime = currentTime;
    blinkInterval = generateRandomNumber(500, 7000);
  }
}

void handleLook(unsigned long currentTime) {
  if (currentTime - lastLookTime >= lookInterval) {
    int direction = random(4);
    if (direction == 0) roboEyes.setPosition(W);
    else if (direction == 1) roboEyes.setPosition(E);
    else if (direction == 2) roboEyes.setPosition(S);
    else roboEyes.setPosition(DEFAULT);
    
    lastLookTime = currentTime;
    lookInterval = generateRandomNumber(500, 10000);
  }
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