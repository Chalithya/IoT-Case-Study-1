#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int temp_pot = A0;    
const int humid_pot = A1; 

const int heat_led = 3;
const int fan_led = 4;

float readTemperature() {
  int val = analogRead(temp_pot);
  return map(val, 0, 1023, 0, 50);
}

float readHumidity() {
  int val = analogRead(humid_pot);
  return map(val, 0, 1023, 0, 100);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(heat_led, OUTPUT);
  pinMode(fan_led, OUTPUT);

  digitalWrite(heat_led, LOW);
  digitalWrite(fan_led, LOW);
}

void loop() {
  float temperature = readTemperature();
  float humidity = readHumidity();

  // Heater control
  if (temperature < 20) {
    digitalWrite(heat_led, HIGH);
  } else if (temperature > 25) {
    digitalWrite(heat_led, LOW);
  }

  // Fan control
  if (temperature > 30 || (temperature > 25 && humidity > 70) || humidity > 90) {
    digitalWrite(fan_led, HIGH);
  } else if (temperature < 25 && humidity < 40) {
    digitalWrite(fan_led, LOW);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  lcd.print("H:");
  lcd.print(digitalRead(heat_led) ? "ON " : "OFF");

  lcd.print("F:");
  lcd.print(digitalRead(fan_led) ? "ON" : "OFF");

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity, 0);
  Serial.print("%, Heater: ");
  Serial.print(digitalRead(heat_led) ? "ON" : "OFF");
  Serial.print(", Fan: ");
  Serial.println(digitalRead(fan_led) ? "ON" : "OFF");

  delay(1000);
}
