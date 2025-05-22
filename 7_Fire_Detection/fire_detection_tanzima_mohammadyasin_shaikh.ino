#include <LiquidCrystal.h>

// LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Sensor and output pins
const int smokePin = A0;
const int tempPin = A1;
const int lightPin = A2;

const int buzzerPin = 11;
const int redLed = 13;
const int greenLed = 12;

// Values
int smokeValue = 0;
float temperatureC = 0;
int lightValue = 0;

// Alert flag
bool alertTriggered = false;

void setup() {
  pinMode(smokePin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(lightPin, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop() {
  smokeValue = analogRead(smokePin);
  int tempRaw = analogRead(tempPin);
  float voltage = tempRaw * (5.0 / 1023.0);
  temperatureC = (voltage - 0.5) * 100.0;
  lightValue = analogRead(lightPin);

  // Debug
  Serial.print("Smoke: ");
  Serial.print(smokeValue);
  Serial.print(" | Temp: ");
  Serial.print(temperatureC);
  Serial.print(" | Light: ");
  Serial.print(lightValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);

  if (smokeValue >= 40 || temperatureC > 50 || lightValue < 100) {
    if (!alertTriggered) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FIRE ALERT");
      lcd.setCursor(0, 1);
      lcd.print("Call Fire: 911");
      alertTriggered = true;
    }
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    tone(buzzerPin, 523);
  } else {
    if (alertTriggered) {
      lcd.clear();
      alertTriggered = false;
    }
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    noTone(buzzerPin);

    lcd.setCursor(0, 0);
    lcd.print("KEEP SMILING :)");
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print((int)temperatureC);
    lcd.print(" C");
  }

  delay(100);
}
