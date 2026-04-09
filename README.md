Overview
AQ-IntelliSense is a self-built, privately funded air quality monitoring system that tracks multiple environmental parameters in real time using an ESP32 microcontroller and displays them on a 0.96" OLED screen.

Parameters Monitored
- CO₂ levels (MQ-135)
- CO levels (MQ-7)
- Temperature & Humidity (DHT22)
- Dust Density / PM2.5 (GP2Y1010AU0F)

Features
- Real-time sensor readings on OLED with smooth scrolling transitions
- Non-blocking read intervals for all 4 sensors simultaneously
- Manual R₀ calibration for MQ-135 and MQ-7 sensors
- Voltage divider circuit for safe ESP32 ADC interfacing
- Designed for scalable integration with additional sensors

Hardware Used
- ESP32-WROOM-32 38Pin Development Board
- MQ-135 (CO₂), MQ-7 (CO), DHT22 (Temp/Humidity)
- GP2Y1010AU0F Dust/PM2.5 Sensor
- 0.96" OLED Display
- 5V 2A External Power Supply
- Breadboard, soldering wire and paste

Tech
- Firmware: Arduino C++ (ESP32)
- Libraries: Adafruit SSD1306, Adafruit GFX, DHT

Photos

![WhatsApp Image 2026-02-17 at 01 41 36](https://github.com/user-attachments/assets/1d2b86b4-7d37-4a9e-9365-8f46f9ad4efa)
![WhatsApp Image 2026-02-17 at 01 41 35](https://github.com/user-attachments/assets/02dce48c-13c0-48f5-ac2c-fe52b01c7184)
![WhatsApp Image 2026-02-17 at 01 41 35 (1)](https://github.com/user-attachments/assets/e2a55c58-83d1-4258-9d34-573b5a12569e)
![WhatsApp Image 2026-02-17 at 01 41 39](https://github.com/user-attachments/assets/a2380f7c-581e-4304-925a-e8cd6fa7a620)
![WhatsApp Image 2026-02-17 at 01 41 38](https://github.com/user-attachments/assets/9d53ebdc-6c90-488c-8b90-9bca5882bc70)
![WhatsApp Image 2026-02-17 at 01 41 38 (1)](https://github.com/user-attachments/assets/42107f7c-a254-44bb-8fe9-841bc947668c)
![WhatsApp Image 2026-02-17 at 01 41 37](https://github.com/user-attachments/assets/55565b94-4fa9-41de-b4df-fe3d0add7a56)

