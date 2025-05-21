#include <LiquidCrystal.h>

// Weather data structure
struct WeatherData {
  String condition;
  float temperature;
  float humidity;
  float windSpeed;
  bool isRaining;
};

// Array of predefined weather scenarios
WeatherData weatherScenarios[] = {
  {"Sunny", 30.5, 45.0, 5.2, false},
  {"Cloudy", 22.3, 65.0, 10.5, false},
  {"Rainy", 18.7, 85.0, 15.3, true},
  {"Stormy", 17.2, 90.0, 25.8, true},
  {"Light Snow", 16.5, 75.0, 8.7, false},
  {"Moderate Snow", 12.8, 80.0, 10.2, false},
  {"Heavy Snow", 7.5, 85.0, 12.5, false},
  {"Freezing", -5.0, 70.0, 15.8, false},
  {"Windy", 15.8, 40.0, 30.2, false},
  {"Hot and Dry", 35.2, 25.0, 3.5, false},
  {"Foggy", 12.5, 95.0, 2.1, false}
};
const int NUM_WEATHER_SCENARIOS = sizeof(weatherScenarios) / sizeof(weatherScenarios[0]);

// Pins
const int HEATER_RED_PIN = 3;
const int HEATER_GREEN_PIN = 5;
const int HEATER_BLUE_PIN = 6;
const int FAN_PIN = 9;
const int WINDOW_PIN = 10;
const int BLINDS_PIN = 11;

// LCD pin setup
LiquidCrystal lcd(12, 13, 4, 7, 8, 2);

// Status variables
bool fanStatus = false;
bool windowStatus = false;
bool blindsStatus = false;
String heaterStatus = "OFF";

// Timer
unsigned long lastDisplayUpdateTime = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 2000;
int currentWeatherIndex = 0;
bool displayWeatherInfo = true;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  pinMode(HEATER_RED_PIN, OUTPUT);
  pinMode(HEATER_GREEN_PIN, OUTPUT);
  pinMode(HEATER_BLUE_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(WINDOW_PIN, OUTPUT);
  pinMode(BLINDS_PIN, OUTPUT);

  setHeaterColor(0, 0, 0);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(WINDOW_PIN, LOW);
  digitalWrite(BLINDS_PIN, LOW);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);

  printWeatherMenu();
}

void loop() {
  // Toggle display
  if (millis() - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
    displayWeatherInfo = !displayWeatherInfo;
    lastDisplayUpdateTime = millis();
    updateLCD();
  }

  if (Serial.available() > 0) {
    processSerialCommands();
  }
}

void setHeaterColor(int red, int green, int blue) {
  analogWrite(HEATER_RED_PIN, red);
  analogWrite(HEATER_GREEN_PIN, green);
  analogWrite(HEATER_BLUE_PIN, blue);
}

void printCurrentWeather() {
  WeatherData current = weatherScenarios[currentWeatherIndex];
  Serial.println("\n===== CURRENT WEATHER =====");
  Serial.print("Condition: "); Serial.println(current.condition);
  Serial.print("Temperature: "); Serial.print(current.temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(current.humidity); Serial.println("%");
  Serial.print("Wind Speed: "); Serial.print(current.windSpeed); Serial.println(" km/h");
  Serial.print("Raining: "); Serial.println(current.isRaining ? "Yes" : "No");
  Serial.println("===========================\n");
}

void applyWeatherBasedAdjustments() {
  WeatherData current = weatherScenarios[currentWeatherIndex];

  // Fan
  if (current.temperature > 27.0 && !current.isRaining) {
    digitalWrite(FAN_PIN, HIGH);
    fanStatus = true;
    Serial.println("Fan turned ON");
  } else {
    digitalWrite(FAN_PIN, LOW);
    fanStatus = false;
    Serial.println("Fan turned OFF");
  }

  // Windows
  if (!current.isRaining && current.temperature > 20.0 && current.temperature < 28.0 && current.windSpeed < 15.0) {
    digitalWrite(WINDOW_PIN, HIGH);
    windowStatus = true;
    Serial.println("Windows OPENED");
  } else {
    digitalWrite(WINDOW_PIN, LOW);
    windowStatus = false;
    Serial.println("Windows CLOSED");
  }

  // Blinds
  if (current.condition == "Sunny" && current.temperature > 25.0) {
    digitalWrite(BLINDS_PIN, LOW);
    blindsStatus = true;
    Serial.println("Blinds CLOSED");
  } else {
    digitalWrite(BLINDS_PIN, HIGH);
    blindsStatus = false;
    Serial.println("Blinds OPENED");
  }

  // Heater
  if (current.temperature >= 18.0) {
    setHeaterColor(0, 0, 0);
    heaterStatus = "OFF";
    Serial.println("Heater OFF");
  } else if (current.temperature >= 15.0) {
    setHeaterColor(0, 255, 0);
    heaterStatus = "LOW";
    Serial.println("Heater LOW");
  } else if (current.temperature >= 10.0) {
    setHeaterColor(255, 255, 0);
    heaterStatus = "MED";
    Serial.println("Heater MED");
  } else {
    setHeaterColor(255, 0, 0);
    heaterStatus = "HIGH";
    Serial.println("Heater HIGH");
  }

  Serial.println("Indoor settings adjusted.");
}

void updateLCD() {
  lcd.clear();

  if (displayWeatherInfo) {
    WeatherData current = weatherScenarios[currentWeatherIndex];
    lcd.setCursor(0, 0);
    lcd.print(current.condition);
    lcd.setCursor(0, 1);
    lcd.print(current.temperature, 1); lcd.print("C ");
    lcd.print(current.humidity, 0); lcd.print("% ");
    lcd.print(current.windSpeed, 1); lcd.print("km/h");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("F:"); lcd.print(fanStatus ? "ON" : "OFF");
    lcd.print(" W:"); lcd.print(windowStatus ? "OPN" : "CLS");
    lcd.print(" B:"); lcd.print(blindsStatus ? "CLS" : "OPN");

    lcd.setCursor(0, 1);
    lcd.print("Heater: "); lcd.print(heaterStatus);
  }
}

void printWeatherMenu() {
  Serial.println("\n===== SELECT A WEATHER SCENARIO =====");
  for (int i = 0; i < NUM_WEATHER_SCENARIOS; i++) {
    Serial.print(i); Serial.print(": "); Serial.println(weatherScenarios[i].condition);
  }
  Serial.println("Type a number (0 to " + String(NUM_WEATHER_SCENARIOS - 1) + ") to select weather.");
  Serial.println("Commands: help, list, display");
}

void processSerialCommands() {
  String command = Serial.readStringUntil('\n');
  command.trim();

  if (command.startsWith("weather")) {
    int index = command.substring(8).toInt();
    if (index >= 0 && index < NUM_WEATHER_SCENARIOS) {
      currentWeatherIndex = index;
      printCurrentWeather();
      applyWeatherBasedAdjustments();
      updateLCD();
    } else {
      Serial.println("Invalid index! Use 0-" + String(NUM_WEATHER_SCENARIOS - 1));
    }
    printWeatherMenu();
  }
  else if (command.toInt() >= 0 && command.toInt() < NUM_WEATHER_SCENARIOS) {
    int index = command.toInt();
    currentWeatherIndex = index;
    printCurrentWeather();
    applyWeatherBasedAdjustments();
    updateLCD();
    printWeatherMenu();
  }
  else if (command == "list") {
    printWeatherMenu();
  }
  else if (command == "display") {
    displayWeatherInfo = !displayWeatherInfo;
    updateLCD();
    Serial.println("LCD display toggled");
  }
  else if (command == "help") {
    Serial.println("\n===== COMMANDS =====");
    Serial.println("Type 0-" + String(NUM_WEATHER_SCENARIOS - 1) + " to select a weather scenario");
    Serial.println("list - Show weather menu again");
    Serial.println("display - Toggle LCD info");
    Serial.println("help - Show this help");
    Serial.println("====================\n");
  }
  else {
    Serial.println("Invalid command. Type 'help' for options.");
  }
}
