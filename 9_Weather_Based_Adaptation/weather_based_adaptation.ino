#include <LiquidCrystal.h>

// Weather data structure
struct WeatherData {
  String condition;  // e.g., "Sunny", "Rainy", "Cloudy", "Snowy", "Windy"
  float temperature; // in Celsius
  float humidity;    // percentage
  float windSpeed;   // in km/h
  bool isRaining;    // true if raining
};

// Array of predefined weather scenarios
WeatherData weatherScenarios[] = {
  {"Sunny", 30.5, 45.0, 5.2, false},
  {"Cloudy", 22.3, 65.0, 10.5, false},
  {"Rainy", 18.7, 85.0, 15.3, true},
  {"Stormy", 17.2, 90.0, 25.8, true},
  {"Light Snow", 16.5, 75.0, 8.7, false},    // Just at heater threshold
  {"Moderate Snow", 12.8, 80.0, 10.2, false}, // Medium heating
  {"Heavy Snow", 7.5, 85.0, 12.5, false},     // High heating
  {"Freezing", -5.0, 70.0, 15.8, false},      // High heating
  {"Windy", 15.8, 40.0, 30.2, false},
  {"Hot and Dry", 35.2, 25.0, 3.5, false},
  {"Foggy", 12.5, 95.0, 2.1, false}
};

const int NUM_WEATHER_SCENARIOS = sizeof(weatherScenarios) / sizeof(weatherScenarios[0]);

// Define pins for RGB LED (heater status)
const int HEATER_RED_PIN = 3;    // Red pin of RGB LED
const int HEATER_GREEN_PIN = 5;  // Green pin of RGB LED
const int HEATER_BLUE_PIN = 6;   // Blue pin of RGB LED

// Define pins for other devices
const int FAN_PIN = 9;           // LED to simulate fan
const int WINDOW_PIN = 10;       // LED to simulate window status
const int BLINDS_PIN = 11;       // LED to simulate blinds status

// LCD pin setup (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 13, 4, 7, 8, 2);

// Status variables
bool fanStatus = false;
bool windowStatus = false;
bool blindsStatus = false;
String heaterStatus = "OFF";

// Timer variables
unsigned long lastWeatherChangeTime = 0;
unsigned long lastDisplayUpdateTime = 0;
const unsigned long WEATHER_CHANGE_INTERVAL = 5000; // 5 seconds in milliseconds
const unsigned long DISPLAY_UPDATE_INTERVAL = 2000; // 2 seconds to alternate display info
int currentWeatherIndex = 0;
bool displayWeatherInfo = true; // Toggle between weather and status info

void setup() {
  Serial.begin(9600);
  
  // Initialize random seed
  randomSeed(analogRead(0)); 
  
  // Initialize the pins
  pinMode(HEATER_RED_PIN, OUTPUT);
  pinMode(HEATER_GREEN_PIN, OUTPUT);
  pinMode(HEATER_BLUE_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(WINDOW_PIN, OUTPUT);
  pinMode(BLINDS_PIN, OUTPUT);
  
  // Turn off all LEDs initially
  setHeaterColor(0, 0, 0); // Heater off (no color)
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(WINDOW_PIN, LOW);
  digitalWrite(BLINDS_PIN, LOW);
  
  // Initialize LCD
  lcd.begin(16, 2);  // 16 columns, 2 rows
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");
  delay(2000);
  
  // Initialize with a random weather to start
  currentWeatherIndex = random(NUM_WEATHER_SCENARIOS);
  printCurrentWeather();
  applyWeatherBasedAdjustments();
  updateLCD();
  
  Serial.println("Weather-Based Adaptation System Started!");
}

void loop() {
  // Check if it's time to change the weather
  if (millis() - lastWeatherChangeTime >= WEATHER_CHANGE_INTERVAL) {
    // Pick a new random weather scenario
    currentWeatherIndex = random(NUM_WEATHER_SCENARIOS);
    lastWeatherChangeTime = millis();
    
    // Print the new weather condition
    printCurrentWeather();
    
    // Apply indoor adjustments based on new weather
    applyWeatherBasedAdjustments();
    
    // Update LCD with new info
    updateLCD();
  }
  
  // Toggle LCD display between weather and status every 2 seconds
  if (millis() - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
    displayWeatherInfo = !displayWeatherInfo;
    lastDisplayUpdateTime = millis();
    updateLCD();
  }
  
  // Add this section to process any serial input
  if (Serial.available() > 0) {
    processSerialCommands();
  }
}

// Helper function to set RGB LED color
void setHeaterColor(int red, int green, int blue) {
  analogWrite(HEATER_RED_PIN, red);
  analogWrite(HEATER_GREEN_PIN, green);
  analogWrite(HEATER_BLUE_PIN, blue);
}

void printCurrentWeather() {
  WeatherData current = weatherScenarios[currentWeatherIndex];
  
  Serial.println("\n===== CURRENT WEATHER =====");
  Serial.print("Condition: ");
  Serial.println(current.condition);
  Serial.print("Temperature: ");
  Serial.print(current.temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(current.humidity);
  Serial.println("%");
  Serial.print("Wind Speed: ");
  Serial.print(current.windSpeed);
  Serial.println(" km/h");
  Serial.print("Raining: ");
  Serial.println(current.isRaining ? "Yes" : "No");
  Serial.println("===========================\n");
}

void applyWeatherBasedAdjustments() {
  WeatherData current = weatherScenarios[currentWeatherIndex];
  
  // Adjust fan based on temperature and rain
  if (current.temperature > 27.0 && !current.isRaining) {
    digitalWrite(FAN_PIN, HIGH);
    fanStatus = true;
    Serial.println("Fan turned ON (hot outside, not raining)");
  } else {
    digitalWrite(FAN_PIN, LOW);
    fanStatus = false;
    Serial.println("Fan turned OFF (cool or raining outside)");
  }
  
  // Adjust windows based on temperature, rain, and wind
  if (!current.isRaining && current.temperature > 20.0 && current.temperature < 28.0 && current.windSpeed < 15.0) {
    digitalWrite(WINDOW_PIN, HIGH);
    windowStatus = true;
    Serial.println("Windows OPENED (pleasant weather)");
  } else {
    digitalWrite(WINDOW_PIN, LOW);
    windowStatus = false;
    Serial.println("Windows CLOSED (bad weather conditions)");
  }
  
  // Adjust blinds based on condition
  if (current.condition == "Sunny" && current.temperature > 25.0) {
    digitalWrite(BLINDS_PIN, LOW);  // Changed from HIGH to LOW
    blindsStatus = true;  // Keep true for CLOSED
    Serial.println("Blinds CLOSED (sunny and hot)");
  } else {
    digitalWrite(BLINDS_PIN, HIGH);  // Changed from LOW to HIGH
    blindsStatus = false;  // Keep false for OPEN
    Serial.println("Blinds OPENED (not too sunny/hot)");
  }
  
  // Adjust heater based on temperature with RGB LED colors
  if (current.temperature >= 18.0) {
    // Heater off - no color
    setHeaterColor(0, 0, 0);
    heaterStatus = "OFF";
    Serial.println("Heater turned OFF (not cold outside)");
  } 
  else if (current.temperature >= 15.0 && current.temperature < 18.0) {
    // Low heat - Green
    setHeaterColor(0, 255, 0);
    heaterStatus = "LOW";
    Serial.println("Heater on LOW setting (slightly cold outside)");
  }
  else if (current.temperature >= 10.0 && current.temperature < 15.0) {
    // Medium heat - Yellow (mix of red and green)
    setHeaterColor(255, 255, 0);
    heaterStatus = "MED";
    Serial.println("Heater on MEDIUM setting (cold outside)");
  }
  else {
    // High heat - Red (temperature < 10.0)
    setHeaterColor(255, 0, 0);
    heaterStatus = "HIGH";
    Serial.println("Heater on HIGH setting (very cold outside)");
  }
  
  Serial.println("Indoor settings adjusted based on weather!");
}

void updateLCD() {
  lcd.clear();
  
  if (displayWeatherInfo) {
    // Display weather info on LCD
    WeatherData current = weatherScenarios[currentWeatherIndex];
    lcd.setCursor(0, 0);
    lcd.print(current.condition);
    
    lcd.setCursor(0, 1);
    lcd.print(current.temperature, 1);
    lcd.print("C ");
    lcd.print(current.humidity, 0);
    lcd.print("% ");
    lcd.print(current.windSpeed, 1);
    lcd.print("km/h");
  } else {
    // Display device status info on LCD
    lcd.setCursor(0, 0);
    lcd.print("F:");
    lcd.print(fanStatus ? "ON" : "OFF");
    lcd.print(" W:");
    lcd.print(windowStatus ? "OPN" : "CLS");
    lcd.print(" B:");
    lcd.print(blindsStatus ? "CLS" : "OPN");
    
    lcd.setCursor(0, 1);
    lcd.print("Heater: ");
    lcd.print(heaterStatus);
  }
}

void processSerialCommands() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command.startsWith("weather")) {
    // Allow manual weather setting, format: "weather 2" to set to index 2
    int index = command.substring(8).toInt();
    if (index >= 0 && index < NUM_WEATHER_SCENARIOS) {
      currentWeatherIndex = index;
      printCurrentWeather();
      applyWeatherBasedAdjustments();
      updateLCD();
    } else {
      Serial.println("Invalid weather index! Use 0-" + String(NUM_WEATHER_SCENARIOS-1));
    }
  } 
  else if (command == "list") {
    // List all available weather scenarios
    Serial.println("\n===== AVAILABLE WEATHER SCENARIOS =====");
    for (int i = 0; i < NUM_WEATHER_SCENARIOS; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(weatherScenarios[i].condition);
    }
    Serial.println("=====================================\n");
  }
  else if (command == "help") {
    Serial.println("\n===== AVAILABLE COMMANDS =====");
    Serial.println("help - Display available commands");
    Serial.println("list - List all weather scenarios");
    Serial.println("weather <index> - Set specific weather scenario");
    Serial.println("display - Force display refresh");
    Serial.println("============================\n");
  }
  else if (command == "display") {
    displayWeatherInfo = !displayWeatherInfo;
    updateLCD();
    Serial.println("LCD display toggled");
  }
}