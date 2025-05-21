# Environmental Monitoring & HVAC Control

## Requirements
- Use sensors to monitor temperature and humidity in each room
- Automatically control simulated fans/heaters (using LEDs) based on predefined thresholds
- Display real-time readings for all rooms on an I2C-enabled LCD

## Rules
- If temperature below 20 turn on heater
- If temperature above 25 turn off heater
- If temperature above 30 turn on fan
- If temperature above 25 and humidity above 70% turn on fan
- If temperature below 25 and humidity below 40% turn off fan
- If humidity above 90% turn on fan 