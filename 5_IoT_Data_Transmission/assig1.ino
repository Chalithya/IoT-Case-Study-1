#include <Keypad.h>
#include <LiquidCrystal.h>
// LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {A1, A2, A3, A4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
bool systemEnabled = false;
String input = "";
String passcode = "1234";
String mode = "Home";
#define TEMP_PIN A0
#define HUMIDITY_PIN A5
#define LIGHT_PIN A6
#define PIR_PIN 10        
#define BUZZER 13
#define RED_LED A2
//Main logic
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Enter Passcode");

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    String inputMode = Serial.readStringUntil('\n');
    inputMode.trim();
    inputMode.toUpperCase();
    if (inputMode == "HOME" || inputMode == "AWAY" || inputMode == "VACATION") {
      mode = inputMode;
      Serial.println("Mode changed remotely to: " + mode);
      lcd.clear();
      lcd.print("Mode: " + mode);
    } else {
      Serial.println("Invalid mode. Use: HOME, AWAY, VACATION");
    }
  }
  char key = keypad.getKey();
  if (key) {
    if (!systemEnabled) {
      if (key == '#') {
        if (input == passcode) {
          systemEnabled = true;
          lcd.clear();
          lcd.print("Access Granted");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Select Mode:");
          lcd.setCursor(0, 1);
          lcd.print("A:H B:A C:V");

          char modeKey = 0;
          while (!modeKey) {
            modeKey = keypad.getKey();
            if (modeKey == 'A') mode = "Home";
            else if (modeKey == 'B') mode = "Away";
            else if (modeKey == 'C') mode = "Vacation";
            else modeKey = 0;
          }

          lcd.clear();
          lcd.print("Mode: " + mode);
          delay(1000);
        } else {
          lcd.clear();
          lcd.print("Wrong Passcode");
          delay(1000);
          lcd.clear();
          lcd.print("Enter Passcode");
        }
        input = "";
      } else if (key == '*') {
        input = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
      } else {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(input);
      }
    }
  }
  if (systemEnabled) {
    float tempC = readTemperature();
    float humidity = analogRead(HUMIDITY_PIN) * 100.0 / 1023.0;
    int lightLevel = analogRead(LIGHT_PIN);
    int motion = digitalRead(PIR_PIN);

    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(tempC, 1);
    lcd.print(" H:"); lcd.print(humidity, 0);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print(mode); lcd.print(" L:"); lcd.print(lightLevel);
    Serial.print("Mode: "); Serial.print(mode);
    Serial.print(" | Temp: "); Serial.print(tempC); Serial.print("C");
    Serial.print(" | Humidity: "); Serial.print(humidity); Serial.print("%");
    Serial.print(" | Light: "); Serial.println(lightLevel);
    if ((mode == "AWAY" || mode == "VACATION") && motion == HIGH) {
      tone(BUZZER, 1000);
      digitalWrite(RED_LED, HIGH);
    } else {
      noTone(BUZZER);
      digitalWrite(RED_LED, LOW);
    }
    delay(1000);
  }
}

float readTemperature() {
  int sensorValue = analogRead(TEMP_PIN);
  float voltage = sensorValue * 5.0 / 1023.0;
  return (voltage - 0.5) * 100.0;
}
