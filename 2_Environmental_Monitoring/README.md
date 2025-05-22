# 🌡️ Environmental Monitoring & HVAC Control System

This project simulates a smart **HVAC (Heating, Ventilation, and Air Conditioning)** control system that monitors temperature and humidity in each room and automatically manages fan and heater status using LEDs. The system displays real-time data on an I2C-enabled LCD.

## ✅ Objectives

- Monitor **temperature** and **humidity** for each room using **DHT11 sensors**
- Control **fans and heaters** with **LEDs** based on environmental conditions
- Display real-time sensor readings on an **I2C LCD**
- Apply decision-making rules for energy efficiency and comfort

## 🧰 Components Used

- Arduino Uno
- DHT11 Temperature and Humidity Sensor (1 per room)
- I2C LCD Display (16x2)
- Red LED (Heater)
- Blue LED (Fan)
- Jumper wires, breadboard, resistors

## 🧠 Control Rules

| Condition                                             | Action             |
|------------------------------------------------------|--------------------|
| Temperature < 20°C                                   | Turn ON heater     |
| Temperature > 25°C                                   | Turn OFF heater    |
| Temperature > 30°C                                   | Turn ON fan        |
| Temperature > 25°C AND Humidity > 70%                | Turn ON fan        |
| Temperature < 25°C AND Humidity < 40%                | Turn OFF fan       |
| Humidity > 90%                                       | Turn ON fan        |

## 📺 Real-Time LCD Display

Displays temperature and humidity of all rooms in the format:

