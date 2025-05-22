# 🌡️ Environmental Monitoring & HVAC Control System

This project simulates a smart **HVAC (Heating, Ventilation, and Air Conditioning)** control system that monitors temperature and humidity in each room and automatically manages fan and heater status using LEDs. The system displays real-time data on an I2C-enabled LCD.

## Objectives

- Monitor **temperature** and **humidity** for each room using **DHT11 sensors**
- Control **fans and heaters** with **LEDs** based on environmental conditions
- Display real-time sensor readings on an **I2C LCD**
- Apply decision-making rules for energy efficiency and comfort

## Components Used

- Arduino Uno
- DHT11 Temperature and Humidity Sensor (1 per room)
- I2C LCD Display (16x2)
- Red LED (Heater)
- Blue LED (Fan)
- Jumper wires, breadboard, resistors

##  Control Rules

| Condition                                             | Action             |
|------------------------------------------------------|--------------------|
| Temperature < 20°C                                   | Turn ON heater     |
| Temperature > 25°C                                   | Turn OFF heater    |
| Temperature > 30°C                                   | Turn ON fan        |
| Temperature > 25°C AND Humidity > 70%                | Turn ON fan        |
| Temperature < 25°C AND Humidity < 40%                | Turn OFF fan       |
| Humidity > 90%                                       | Turn ON fan        |


- Uses **I2C protocol** for simplified LCD wiring
- Updates continuously with latest sensor values

## ⚙️ How to Use

1. Connect the components as per the Tinkercad circuit diagram.
2. Upload the Arduino code.
3. Observe LED behavior based on temperature and humidity values.
4. Check real-time values on the LCD screen.

## 🔗 Tinkercad Simulation Links

- 🔧 **Tinkercad - Kabir**: [Click here to view](https://www.tinkercad.com/things/16kmXZnSeAV-copy-of-20-dht11-temperature-and-humidity-sensor/editel?returnTo=%2Fthings%2F16kmXZnSeAV-copy-of-20-dht11-temperature-and-humidity-sensor&sharecode=WJGg8xuPlnL_m_0ew0XYyJNQWKNeAQ-nNBFEhfM3PUI)

- 🔧 **Tinkercad - Siddhi**: [Click here to view](https://www.tinkercad.com/things/8GWsDSycRc1-newwass/editel?returnTo=https%3A%2F%2Fwww.tinkercad.com%2Fdashboard)

---



