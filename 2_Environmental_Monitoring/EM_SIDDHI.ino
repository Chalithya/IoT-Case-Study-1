#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
// You might need to try 0x3F if 0x27 doesn't work.
LiquidCrystal_I2C lcd(0x20, 16, 2);

#define HEATER_LED_PIN 8 // Red LED for heater
#define FAN_LED_PIN 9    // Green LED for fan
#define TEMP_POT_PIN A0  // Potentiometer for Temperature
#define HUMIDITY_POT_PIN A1 // Potentiometer for Humidity

void setup() {
  Serial.begin(9600); // Initialize Serial Monitor for debugging
  lcd.init();        // Initialize the LCD
  lcd.backlight();   // Turn on the backlight

  lcd.print("ENV Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("Loading...");
  delay(2000);
  lcd.clear();

  pinMode(HEATER_LED_PIN, OUTPUT);
  pinMode(FAN_LED_PIN, OUTPUT);

  Serial.println("--- System Initialized ---");
  Serial.println("Monitoring Temp and Humidity.");
  Serial.println("Controlling Heater (Red LED) and Fan (Green LED).");
  Serial.println("--------------------------");
}

void loop() {
  // Read raw analog values (0-1023) from potentiometers
  int tempRaw = analogRead(TEMP_POT_PIN);
  int humidityRaw = analogRead(HUMIDITY_POT_PIN);

  // Map raw values to realistic ranges for display and logic
  // Temperature mapped to 10-40 degrees Celsius (adjustable range)
  float temperatureC = map(tempRaw, 0, 1023, 10, 40);
  // Humidity mapped to 0-100 percent
  int humidityPercent = map(humidityRaw, 0, 1023, 0, 100);

  // --- Control Logic for Heater and Fan ---
  // The order of these 'if' statements matters to handle potential conflicts
  // and ensure the desired behavior. We'll set the states and then print them.

  // First, assume both are OFF, then turn them ON if rules apply.
  // This helps prevent "sticky" ON states from previous loop iterations.
  digitalWrite(HEATER_LED_PIN, LOW);
  digitalWrite(FAN_LED_PIN, LOW);

  // Heater Control Logic:
  // Rule a: If temperature below 20 turn on heater
  if (temperatureC < 20) {
    digitalWrite(HEATER_LED_PIN, HIGH);
  }
  // Rule b: If temperature above 25 turn off heater
  // This acts as an override if the heater was turned on by Rule 'a'
  // but temperature has now risen above 25.
  if (temperatureC > 25) {
    digitalWrite(HEATER_LED_PIN, LOW);
  }

  // Fan Control Logic:
  // The fan rules are cumulative, meaning any condition can turn it ON.
  // The OFF rule will then override if its conditions are met.

  // Rule c: If temperature above 30 turn on fan
  if (temperatureC > 30) {
    digitalWrite(FAN_LED_PIN, HIGH);
  }
  // Rule d: If temperature above 25 and humidity above 70% turn on fan
  if (temperatureC > 25 && humidityPercent > 70) {
    digitalWrite(FAN_LED_PIN, HIGH);
  }
  // Rule f: If humidity above 90% turn on fan
  if (humidityPercent > 90) {
    digitalWrite(FAN_LED_PIN, HIGH);
  }

  // Rule e: If temperature below 25 and humidity below 40% turn off fan
  // This rule acts as a collective OFF switch for the fan.
  // It will override any ON conditions if it applies.
  if (temperatureC < 25 && humidityPercent < 40) {
    digitalWrite(FAN_LED_PIN, LOW);
  }


  // --- Output to Serial Monitor ---
  Serial.print("Temp: ");
  Serial.print(temperatureC, 1); // 1 decimal place
  Serial.print(" C (raw: ");
  Serial.print(tempRaw);
  Serial.print(") | Hum: ");
  Serial.print(humidityPercent);
  Serial.print(" % (raw: ");
  Serial.print(humidityRaw);
  Serial.print(") | ");

  Serial.print("Heater: ");
  Serial.print(digitalRead(HEATER_LED_PIN) ? "ON" : "OFF"); // Reads the current state of the pin
  Serial.print(" | Fan: ");
  Serial.println(digitalRead(FAN_LED_PIN) ? "ON" : "OFF");   // Reads the current state of the pin

  // --- Display readings on LCD ---
  lcd.clear(); // Clear the LCD for fresh display
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureC, 1); // 1 decimal place
  lcd.print((char)223);      // Degree symbol (ASCII 223)
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidityPercent);
  lcd.print("%");

  delay(1000); // Update every 1 second
}