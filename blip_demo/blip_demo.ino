#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "esp_random.h"
#include "bootloader_random.h"
#include "SoundLibrary.h"
#include "lib/FluxGarage_RoboEyes.h"
#include <Wire.h>
#include <MPU6050_light.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define TOUCH_PIN 2
#define BUZZER_PIN 4
#define buttonPin 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TwoWire MPUWire = TwoWire(1);
MPU6050 mpu(MPUWire);
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

const char* ssid = "Nevalink_511";
const char* password = "YOUR_WIFI_PASSWORD";
const char* apiKey = "YOUR_OPENWEATHER_API_KEY";
const char* city = "Moscow";

String weatherDescription = "";
float temperature = 0;
int weatherCondition = 0;

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 600000;

bool showingWeather = false;
unsigned long weatherScreenStart = 0;
const unsigned long WEATHER_SCREEN_TIME = 5000;

bool lastButtonState = LOW;
bool currentButtonState = LOW;

enum MPUDirection {
  MPU_NONE,
  MPU_LEFT,
  MPU_RIGHT,
  MPU_DOWN,
  MPU_UP
};

MPUDirection currentMPUDirection = MPU_NONE;

bool sweatingActive = false;
bool sweatingTriggered = false;

void setup() {
  Serial.begin(115200);
  bootloader_random_enable();

  pinMode(TOUCH_PIN, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  soundPlayer.begin(BUZZER_PIN);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed!"));
    for (;;) {
      delay(10);
    }
  }

  mpuSetup();

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setPosition(DEFAULT);
  roboEyes.setMood(DEFAULT);
  roboEyes.open();

  connectToWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    fetchWeather();
    displayWeather();

      // Check temperature and enable sweat if above 25°C
    if (temperature > 25.0) {
      roboEyes.setMood(TIRED);
      roboEyes.setSweat(true);
      sweatingActive = true;
      sweatingTriggered = true;
      Serial.println("Sweat activated! Temperature: " + String(temperature) + "°C");
    }
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("No Wi-Fi!");
    display.println("Please connect");
    display.println("to network");
    display.display();
  }

  lastBlinkTime = millis();
  lastLookTime = millis();
  lastWeatherUpdate = millis();
}

void loop() {
  unsigned long currentTime = millis();

  mpu.update();

  readTouch(currentTime);
  updateRobotState(currentTime);

  currentButtonState = digitalRead(buttonPin);

  if (currentButtonState == HIGH && lastButtonState == LOW && !showingWeather) {
    showingWeather = true;
    weatherScreenStart = currentTime;

    fetchWeather();
    displayWeather();
  }

  lastButtonState = currentButtonState;

  if (showingWeather) {
    if (currentTime - weatherScreenStart >= WEATHER_SCREEN_TIME) {
      showingWeather = false;

      display.clearDisplay();
      display.display();

      roboEyes.setPosition(DEFAULT);
      roboEyes.open();
    }
  }

  if (!showingWeather) {
    roboEyes.update();
    gravityEyesDirection();
    handleBlink(currentTime);

    if (currentMPUDirection == MPU_NONE) {
      handleLook(currentTime);
    }
  }

  handleSound(currentTime);

  // Periodic sweat check - every 60 seconds
  static unsigned long lastSweatCheck = 0;
  if (currentTime - lastSweatCheck >= 60000) {
    if (WiFi.status() == WL_CONNECTED) {
      fetchWeather();
      updateSweatState();
    }
    lastSweatCheck = currentTime;
  }

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

      if (currentTime - lastProgressTime > 500) {
        float progress = ((float)(currentTime - touchStartTime) / HOLD_TIME) * 100;

        Serial.print("Holding: ");
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

    if (direction == 0) {
      roboEyes.setPosition(ROBO_W);
    } else if (direction == 1) {
      roboEyes.setPosition(ROBO_E);
    } else if (direction == 2) {
      roboEyes.setPosition(ROBO_S);
    } else {
      roboEyes.setPosition(DEFAULT);
    }

    lastLookTime = currentTime;
    lookInterval = generateRandomNumber(500, 10000);
  }
}

uint32_t generateRandomNumber(uint32_t min, uint32_t max) {
  if (min >= max) {
    return min;
  }

  uint32_t range = max - min + 1;
  uint32_t limit = UINT32_MAX - (UINT32_MAX % range);
  uint32_t random_num;

  do {
    random_num = esp_random();
  } while (random_num >= limit);

  return min + (random_num % range);
}


// ================================ MPU ==========================================
void mpuSetup() {
  MPUWire.begin(25, 26);

  byte status = mpu.begin();

  if (status != 0) {
    Serial.print("MPU6050 error: ");
    Serial.println(status);

    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 connected!");
  Serial.println("Keep MPU6050 completely still...");
  delay(1000);

  mpu.calcOffsets();

  Serial.println("MPU6050 calibration complete!");
}

void gravityEyesDirection() {
  float angleX = mpu.getAngleX();
  float angleY = mpu.getAngleY();

  static unsigned long lastMPUPrint = 0;

  if (millis() - lastMPUPrint >= 250) {
    Serial.print("Angle X: ");
    Serial.print(angleX, 1);
    Serial.print(" | Angle Y: ");
    Serial.println(angleY, 1);

    lastMPUPrint = millis();
  }

  const float THRESHOLD = 25.0;

  MPUDirection newDirection = MPU_NONE;

  if (angleY > THRESHOLD) {
    newDirection = MPU_RIGHT;
  }
  else if (angleY < -THRESHOLD) {
    newDirection = MPU_LEFT;
  }
  else if (angleX > THRESHOLD) {
    newDirection = MPU_DOWN;
  }
  else if (angleX > THRESHOLD) {
    newDirection = MPU_DOWN;
  }
  else if (angleX < -THRESHOLD) {
    newDirection = MPU_UP;
  }
  else {
    newDirection = MPU_NONE;
  }

  if (newDirection != currentMPUDirection) {
    currentMPUDirection = newDirection;

    switch (currentMPUDirection) {
      case MPU_UP:
        Serial.println("MPU DIRECTION: RIGHT");
        roboEyes.setCuriosity(true);
        roboEyes.setPosition(ROBO_E);
        break;

      case MPU_RIGHT:
        Serial.println("MPU DIRECTION: DOWN");
        roboEyes.setPosition(ROBO_S);
        roboEyes.anim_heartEyes(); 
        break;

      case MPU_DOWN:
        Serial.println("MPU DIRECTION: LEFT");
        roboEyes.setPosition(ROBO_W);
        roboEyes.setCuriosity(true);
        break;

      case MPU_LEFT:
        Serial.println("MPU DIRECTION: UP");
        roboEyes.setPosition(ROBO_N);
        roboEyes.setMood(DEFAULT);
        break;

      case MPU_NONE:
        Serial.println("MPU DIRECTION: CENTER");
        roboEyes.setPosition(DEFAULT);
        roboEyes.setCuriosity(false);
        roboEyes.setMood(DEFAULT);
        break;
    }
  }
}

void connectToWiFi() {
  Serial.println("[+] Connecting to Wifi...");

  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n [+] Connected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n [-] Failed to connect to Wi-Fi");
  }
}

void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi not connected. Skipping weather update.");
    return;
  }

  HTTPClient http;

  String url = "http://api.openweathermap.org/data/2.5/weather?q=" +
               String(city) +
               "&appid=" +
               String(apiKey) +
               "&units=metric";

  http.begin(url);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();

    Serial.println("[+] Weather data received!");

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      temperature = doc["main"]["temp"];

      const char* desc = doc["weather"][0]["description"];
      weatherDescription = String(desc);

      int conditionId = doc["weather"][0]["id"];
      weatherCondition = getWeatherCondition(conditionId);

      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println("°C");

      Serial.print("Weather: ");
      Serial.println(weatherDescription);

      Serial.print("Condition code: ");
      Serial.println(weatherCondition);
    } else {
      Serial.println("[-] Failed to parse JSON");
    }
  } else {
    Serial.print("[-] HTTP error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void updateSweatState() {
  if (temperature > 25.0) {
    if (!sweatingActive) {
      roboEyes.setSweat(true);
      sweatingActive = true;
      Serial.println("Sweat ON - Temperature: " + String(temperature) + "°C");
    }
  } else {
    if (sweatingActive) {
      roboEyes.setSweat(false);
      sweatingActive = false;
      Serial.println("Sweat OFF - Temperature: " + String(temperature) + "°C");
    }
  }
}

int getWeatherCondition(int conditionId) {
  if (conditionId >= 200 && conditionId < 300) return 3;
  if (conditionId >= 300 && conditionId < 600) return 2;
  if (conditionId >= 600 && conditionId < 700) return 4;
  if (conditionId >= 700 && conditionId < 800) return 5;
  if (conditionId == 800) return 0;
  if (conditionId > 800 && conditionId < 900) return 1;

  return 0;
}

void displayWeather() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("Weather: " + String(city));

  display.setTextSize(2);
  display.setCursor(0, 24);
  display.print(temperature, 1);
  display.println("°C");

  display.setTextSize(1);
  display.setCursor(0, 48);

  if (weatherDescription.length() > 16) {
    weatherDescription = weatherDescription.substring(0, 16);
  }

  display.println(weatherDescription);

  drawWeatherIcon(weatherCondition);

  display.display();
}

void drawWeatherIcon(int condition) {
  int iconX = 100;
  int iconY = 18;
  int iconSize = 20;

  switch (condition) {
    case 0:
      display.drawCircle(iconX + 10, iconY + 10, 8, SSD1306_WHITE);
      display.drawLine(iconX + 10, iconY, iconX + 10, iconY - 3, SSD1306_WHITE);
      display.drawLine(iconX + 10, iconY + 20, iconX + 10, iconY + 23, SSD1306_WHITE);
      display.drawLine(iconX, iconY + 10, iconX - 3, iconY + 10, SSD1306_WHITE);
      display.drawLine(iconX + 20, iconY + 10, iconX + 23, iconY + 10, SSD1306_WHITE);
      display.drawLine(iconX + 3, iconY + 3, iconX + 1, iconY + 1, SSD1306_WHITE);
      display.drawLine(iconX + 17, iconY + 3, iconX + 19, iconY + 1, SSD1306_WHITE);
      display.drawLine(iconX + 3, iconY + 17, iconX + 1, iconY + 19, SSD1306_WHITE);
      display.drawLine(iconX + 17, iconY + 17, iconX + 19, iconY + 19, SSD1306_WHITE);
      break;

    case 1:
      display.fillCircle(iconX + 8, iconY + 12, 6, SSD1306_WHITE);
      display.fillCircle(iconX + 15, iconY + 10, 8, SSD1306_WHITE);
      display.fillCircle(iconX + 22, iconY + 12, 6, SSD1306_WHITE);
      display.fillRect(iconX + 5, iconY + 12, 20, 5, SSD1306_WHITE);
      break;

    case 2:
      display.fillCircle(iconX + 8, iconY + 10, 6, SSD1306_WHITE);
      display.fillCircle(iconX + 15, iconY + 8, 8, SSD1306_WHITE);
      display.fillCircle(iconX + 22, iconY + 10, 6, SSD1306_WHITE);
      display.fillRect(iconX + 5, iconY + 10, 20, 5, SSD1306_WHITE);
      display.drawLine(iconX + 5, iconY + 18, iconX + 4, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 12, iconY + 18, iconX + 11, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 19, iconY + 18, iconX + 18, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 26, iconY + 18, iconX + 25, iconY + 22, SSD1306_WHITE);
      break;

    case 3:
      display.fillCircle(iconX + 8, iconY + 10, 6, SSD1306_WHITE);
      display.fillCircle(iconX + 15, iconY + 8, 8, SSD1306_WHITE);
      display.fillCircle(iconX + 22, iconY + 10, 6, SSD1306_WHITE);
      display.fillRect(iconX + 5, iconY + 10, 20, 5, SSD1306_WHITE);
      display.drawLine(iconX + 12, iconY + 15, iconX + 8, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 8, iconY + 22, iconX + 14, iconY + 20, SSD1306_WHITE);
      display.drawLine(iconX + 14, iconY + 20, iconX + 10, iconY + 28, SSD1306_WHITE);
      break;

    case 4:
      display.fillCircle(iconX + 8, iconY + 10, 6, SSD1306_WHITE);
      display.fillCircle(iconX + 15, iconY + 8, 8, SSD1306_WHITE);
      display.fillCircle(iconX + 22, iconY + 10, 6, SSD1306_WHITE);
      display.fillRect(iconX + 5, iconY + 10, 20, 5, SSD1306_WHITE);
      display.drawLine(iconX + 5, iconY + 18, iconX + 3, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 5, iconY + 22, iconX + 3, iconY + 18, SSD1306_WHITE);
      display.drawLine(iconX + 12, iconY + 18, iconX + 10, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 12, iconY + 22, iconX + 10, iconY + 18, SSD1306_WHITE);
      display.drawLine(iconX + 19, iconY + 18, iconX + 17, iconY + 22, SSD1306_WHITE);
      display.drawLine(iconX + 19, iconY + 22, iconX + 17, iconY + 18, SSD1306_WHITE);
      break;

    case 5:
      display.fillCircle(iconX + 8, iconY + 10, 6, SSD1306_WHITE);
      display.fillCircle(iconX + 15, iconY + 8, 8, SSD1306_WHITE);
      display.fillCircle(iconX + 22, iconY + 10, 6, SSD1306_WHITE);
      display.fillRect(iconX + 5, iconY + 10, 20, 5, SSD1306_WHITE);
      display.drawLine(iconX + 2, iconY + 18, iconX + 28, iconY + 18, SSD1306_WHITE);
      display.drawLine(iconX + 5, iconY + 22, iconX + 25, iconY + 22, SSD1306_WHITE);
      break;

    default:
      break;
  }
}

void showWeatherOnScreen() {
  displayWeather();
  delay(5000);
}