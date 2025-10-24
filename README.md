# ESP32 Servo Web Controller 2D Robot

## Overview
This project demonstrates how to build a **Wi-Fi–controlled dual-servo system** using an **ESP32 microcontroller** and Arduino framework.  
It hosts a responsive **web interface** that allows you to adjust two servo angles using sliders and a Send button — with no page reloads.  
Ideal for **robotic arms, IoT actuators, or web-based motion controllers**.

---

## Features
- Web-based UI with two sliders and a Send button
- Real-time servo control over Wi-Fi (STA or AP mode)
- JSON status endpoint `/status`
- Serial debug logging for all HTTP events and servo angles
- Built-in mDNS (`http://esp32.local`) and AP fallback (`ESP32-Servo`)

---

## Hardware Setup

| Component | Description |
|------------|--------------|
| **ESP32 DevKit V1** | Microcontroller board |
| **Servo 1 (GPIO18)** | Signal pin for servo 1 |
| **Servo 2 (GPIO19)** | Signal pin for servo 2 |
| **External 5V Power Supply** | ≥ 2A, common ground with ESP32 |
| **4700µF Capacitor + 0.1µF** | Across 5V–GND near servos |

**Important:** Do not power servos from ESP32 USB 5V. Always use an external supply and connect GNDs together.

---

## Wiring Diagram

| Servo | Signal Pin | +5V | GND |
|--------|-------------|-----|-----|
| Servo 1 | GPIO18 | External 5V | Common GND |
| Servo 2 | GPIO19 | External 5V | Common GND |

---

## Installation

### Requirements
- Arduino IDE (v2.x recommended)
- ESP32 Board Package (`esp32 by Espressif Systems`)
- Libraries: `ESP32Servo`, `WiFi`, `WebServer`, `ESPmDNS`

### Steps
1. Open the project in Arduino IDE.
2. Configure Wi-Fi credentials in the top of the sketch:
   ```cpp
   const char* WIFI_SSID = "YourWiFiName";
   const char* WIFI_PASS = "YourPassword";
   ```
3. Select **Board**: ESP32 Dev Module
4. Upload to your ESP32.
5. Open Serial Monitor at **115200 baud**.

---

## Usage

1. After upload, open Serial Monitor. You will see:
   ```
   Wi-Fi connected: IP 192.168.x.x
   HTTP server started
   ```
2. Open the shown IP or `http://esp32.local` in your browser.
3. Move sliders and press **Send**.
4. Observe servo movement and serial logs.

---

## API Endpoints

| Endpoint | Description | Example |
|-----------|--------------|----------|
| `/` | Main UI page | `http://esp32.local/` |
| `/set?s1=90&s2=45` | Set servo angles | `http://esp32.local/set?s1=90&s2=45` |
| `/status` | Get JSON status | `{"s1":90,"s2":45}` |

---

## Troubleshooting

- **No movement:** Check power supply and common ground.
- **ESP resets:** Add capacitor and use ≥2A 5V source.
- **Can't access esp32.local:** Install Bonjour/mDNS or use printed IP.
- **Servo twitching:** Reduce PWM range (`usMin/usMax` = 600–2400).

---

## Repository Info

**Repository Name:** `esp32-servo-web-controller`  
**Description:** Web-based dual-servo controller using ESP32 and Arduino framework.  
**Topics:** `esp32`, `arduino`, `servo`, `iot`, `webserver`, `robotics`, `automation`, `wifi-control`

**Author:** Mohammed Shehsin  
**License:** MIT

---

## License
MIT License © 2025 Mohammed Shehsin
