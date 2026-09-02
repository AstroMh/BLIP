#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "esp_random.h"
#include "bootloader_random.h" 
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

unsigned long lastBlinkTime = 0;
unsigned long lastLookTime = 0;

uint32_t blinkInterval = 2000;
uint32_t lookInterval = 5000;

void setup() {

  bootloader_random_enable();

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

  roboEyes.update();
  blinkRandomly();
  lookRandomly();


}

uint32_t generateRandomNumber(uint32_t min, uint32_t max) {
  if (min >= max) return min;

  uint32_t range = max - min + 1;
  uint32_t limit = UINT32_MAX - (UINT32_MAX % range);
  uint32_t random_num;

  do {
    random_num = esp_random(); 
  } while (random_num  >= limit);

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
      roboEyes.setPosition(N);
    } else {
      roboEyes.setPosition(DEFAULT);
    }

    lastLookTime = millis();
    lookInterval = generateRandomNumber(500, 10000);

  }
}