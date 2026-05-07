# 🚗 Vehicle Safety System using STM32

An STM32-based automotive safety system that detects vehicle crashes using MPU6050 accelerometer data and sends real-time GPS location alerts over MQTT using ESP8266. Crash events are logged into EEPROM to mimic automotive black-box systems used in ADAS applications.

---

# 📌 Features

- 🚨 Crash Detection using MPU6050 Accelerometer
- 📍 Real-Time GPS Tracking using NEO-6M
- 🌐 MQTT Alert System using ESP8266
- 💾 EEPROM-based Black Box Crash Logging
- 🔌 UART & I2C Communication
- ⚡ Real-Time Embedded System using STM32

#  Hardware Components

| Component | Description |
|----------|-------------|
| STM32 Nucleo F401RE | Main Microcontroller |
| MPU6050 | Accelerometer + Gyroscope |
| ESP8266 | WiFi + MQTT Communication |
| NEO-6M GPS | GPS Tracking Module |
| AT24C32 EEPROM | Crash Data Logging |
| Breadboard & Jumper Wires | Prototyping |

---

# Hardware Connections

## MPU6050 → STM32 (I2C)

| MPU6050 | STM32 |
|---------|-------|
| VCC | 3.3V |
| GND | GND |
| SDA | PB7 |
| SCL | PB6 |

---

## ESP8266 → STM32 (UART)

| ESP8266 | STM32 |
|---------|-------|
| TX | PA3 |
| RX | PA2 |
| GND | GND |

---

## GPS → ESP8266

| GPS | ESP8266 |
|-----|----------|
| TX | D5 |
| RX | D6 |
| VCC | 3.3V |
| GND | GND |

---

# Working Principle

1. STM32 continuously reads acceleration values from MPU6050.
2. Crash detection algorithm calculates resultant acceleration.
3. If acceleration exceeds threshold (>3.5g):
   - Crash detected
   - GPS coordinates fetched
   - Alert sent via MQTT
   - Crash data stored in EEPROM


---

# 📡 MQTT Payload Example

```json
{
  "event": "CRASH",
  "lat": 28.613900,
  "lon": 77.209000
}
