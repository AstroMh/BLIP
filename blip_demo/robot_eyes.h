#ifndef ROBOT_EYES_H
#define ROBOT_EYES_H

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <FluxGarage_RoboEyes.h>

class RobotEyesController {
  private:
    Adafruit_SSD1306& _display;
    RoboEyes<Adafruit_SSD1306> _roboEyes;
    
    unsigned long _lastBlinkTime;
    unsigned long _lastLookTime;
    uint32_t _blinkInterval;
    uint32_t _lookInterval;
    
    uint32_t generateRandomNumber(uint32_t min, uint32_t max);
    void blinkRandomly();
    void lookRandomly();
    
  public:
    RobotEyesController(Adafruit_SSD1306& display);
    void begin(int screenWidth, int screenHeight);
    void update();
};

#endif