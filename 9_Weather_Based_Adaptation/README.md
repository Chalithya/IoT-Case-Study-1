# Question 9. Weather-Based Adaptation:

# Weather-Based Adaptation (Optional)
## Requirements
- Simulate pulling external weather data (manually input via Serial) to adjust indoor settings (e.g., turn off fans if it's raining)

# LED Indicators for Smart Home Weather Adaptation System

## Device Status LEDs

| Device  | LED Color | ON State | OFF State |
|---------|-----------|----------|-----------|
| Fan     | Blue      | ON when temperature > 27°C and not raining | OFF when cool or raining |
| Windows | Green     | ON when weather is pleasant (20-28°C, no rain, wind < 15 km/h) | OFF during bad weather conditions |
| Blinds  | Yellow    | ON when blinds are CLOSED (during sunny, hot conditions > 25°C) | OFF when blinds are OPEN (not too sunny/hot) |

## Heater Status RGB LED

The heater uses an RGB LED to indicate different heating levels:

| Heater Status | LED Color | Temperature Range | Description |
|---------------|-----------|-------------------|-------------|
| OFF           | No color  | ≥ 18°C           | Comfortable temperature, no heating needed |
| LOW           | Green     | 15-18°C          | Slight heating required |
| MEDIUM        | Yellow    | 10-15°C          | Moderate heating required |
| HIGH          | Red       | < 10°C           | Maximum heating required for freezing conditions |

## Pin Configuration

```
HEATER_RED_PIN = 3    // Red component of RGB LED
HEATER_GREEN_PIN = 5  // Green component of RGB LED
HEATER_BLUE_PIN = 6   // Blue component of RGB LED
FAN_PIN = 9           // Blue LED
WINDOW_PIN = 10       // Green LED
BLINDS_PIN = 11       // Yellow LED
```

## LCD Display Information
The LCD screen alternates between displaying:
- Weather information (condition, temperature, humidity, wind speed)
- Device status (Fan, Windows, Blinds, Heater)


# URL for Tinkercad
https://www.tinkercad.com/things/4P0HhT3Gm5Y-final-submission-weather-based-adaptation-simulation?sharecode=N08R3jZL_4Ga4hNc4-XGbK12hpbKARA3EurxrdZKY0c