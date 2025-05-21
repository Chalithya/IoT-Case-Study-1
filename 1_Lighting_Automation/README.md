# Lighting Automation

## Requirements
- Control lights in 4 rooms using a combination of motion sensors (PIR) and ambient light sensors (photoresistors)
- Add a timer (using an RTC module) to enable scheduling

## Rules
- If low ambient light and motion detected turn on lights
- If no motion detected since 5 mins, dim lights
- If high ambient light or no motion detected since 15 mins, turn off lights
- If time after 9 pm, dim lights
- If time after 11 pm, turn off lights 