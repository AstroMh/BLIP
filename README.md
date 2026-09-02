# 🤖 BLIP - Basic Little Interactive Pet

BLIP is an interactive desktop robot with personality! It reacts to touch, motion, and sound with cute animated eyes and sounds. When connected to Wi-Fi, BLIP becomes a voice-activated AI assistant powered by free cloud services.

## ✨ Features

### Offline Personality (The "Dumb" Brain)
- **👀 Animated RoboEyes**: Expresses emotions like Happy, Angry, Sleepy, Scared, and more on an OLED display
- **✋ Touch Reactions**: Responds to being petted or poked with personality
- **🔄 Motion Detection**: Reacts to shaking, tilting, or being lifted with a "scared" expression
- **🎤 Sound Sensitivity**: Reacts to loud noises or claps
- **💤 Auto-Sleep Mode**: Goes to sleep after inactivity to save power
- **🔊 Simple Sounds**: Makes cute "blip" sounds using a buzzer

### Online Intelligence (The "Smart" Brain)
- **🗣️ Voice AI Assistant**: Ask questions and get spoken responses
- **🌤️ Weather Updates**: Displays real-time weather with icons
- **😊 Emoji Display**: Shows fun emojis for different moods
- **🎯 Free & Private**: Uses HuggingFace and open-source models with no API costs

## 🎯 The Vision

BLIP is designed to be a fun, rewarding project for learning embedded systems. It combines:

- **Hardware Integration**: Sensors, displays, and audio
- **Embedded Programming**: ESP32 with Arduino/ESP-IDF
- **Cloud Services**: Free AI APIs for voice interaction
- **3D Design**: Custom enclosure for a polished finish

## 🛠️ Hardware Requirements

### Core Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32-S3 (with PSRAM) | 1 | Main controller (ESP32 works for offline features) |
| SSD1306 OLED Display (128x64, I2C) | 1 | Animated RoboEyes |
| MPU6050 Accelerometer | 1 | Motion detection |
| TTP223 Touch Sensor | 1 | Touch interactions |
| Microphone Module (I2S) | 1 | Voice input for GPT |
| MAX98357A Amplifier + Speaker | 1 | Voice output and sounds |
| Buzzer | 1 | Simple "blip" sounds |
| Push Button | 1 | Trigger voice recording |
| LED + Resistor | 1 | Status indicator |
| 3.7V LiPo Battery (optional) | 1 | Portable power |
| Jumper Wires | Several | Connections |
| Breadboard | 1 | Prototyping |

### 3D Printed Parts
- Custom enclosure with screen cutout
- Mounting points for all components

## 📦 Software Libraries

Install these libraries in your Arduino IDE:

```cpp
// Core Libraries
#include <Wire.h>                 // I2C communication
#include <Adafruit_GFX.h>         // Graphics for OLED
#include <Adafruit_SSD1306.h>     // OLED display driver
#include <FluxGarage_RoboEyes.h>  // Animated RoboEyes library
#include <MPU6050.h>              // Motion sensor
#include <WiFi.h>                 // Internet connectivity
#include <WiFiClientSecure.h>     // Secure HTTPS connections
#include <ArduinoJson.h>          // JSON parsing for APIs
```

## 🎮 Interaction Modes

### Offline Mode (No Wi-Fi Needed)

| User Action | BLIP Reaction |
|-------------|---------------|
| Touch/pet BLIP | Happy eyes + blip sound |
| Shake or tilt | Scared eyes + alarm |
| Lift off table | Scared eyes + droopy expression |
| Loud noise | Angry/shocked eyes |
| Ignore for 2 minutes | Sleepy eyes → Asleep |

### Online Mode (Wi-Fi Connected)

| User Action | BLIP Reaction |
|-------------|---------------|
| Press button + speak | Recording animation → Thinking → Response |
| Ask "What's the weather?" | Shows temperature + weather icon |
| Ask "How are you?" | Happy eyes + cheerful response |
| Say "Goodbye" | Waving animation + goodbye sound |


## 📚 Learning Resources

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [RoboEyes Library](https://github.com/FluxGarage/FluxGarage_RoboEyes)
- [HuggingFace Inference](https://huggingface.co/docs/api-inference/index)
- [MPU6050 Tutorial](https://randomnerdtutorials.com/esp32-mpu6050-accelerometer-gyroscope-arduino/)

## 🤝 Contributing

BLIP is open-source and welcomes contributions!

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [ESP32 Pet Robot Project](https://github.com/SukunDev/ESP32-Pet-Robot) for the offline interaction inspiration
- [HuggingFace](https://huggingface.co/) for providing free AI inference
- All open-source library maintainers

---

## 🔮 Future Enhancements

- [ ] Voice wake word (like "Hey BLIP")
- [ ] Custom wake-up animation
- [ ] Bluetooth remote control
- [ ] Multiple personality modes
- [ ] Cloud synchronization of interactions
- [ ] AI-generated responses
- [ ] Battery level monitoring
- [ ] OTA (Over-The-Air) updates
- [ ] Customizable personality settings via mobile app
