#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIR_PINS[4] = {2, 4, 6, 7};
const int LDR_PINS[4] = {A0, A1, A2, A3};
const int LED_PINS[4] = {3, 5, 9, 10};

const int dimValue = 50;
const int fullBrightness = 255;
const int offValue = 0;

const int ldrDarkThreshold = 400;
const int ldrBrightThreshold = 500;

const unsigned long dimAfter = 5;    // 5 simulated minutes
const unsigned long offAfter = 15;   // 15 simulated minutes

unsigned long lastSimTime = 0;
int simHour = 20;
int simMinute = 0;

unsigned long lastMotionSimulatedTime[4] = {0};
bool hasMotion[4] = {false};
bool wasDark[4] = {false};

void setup() {
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 4; i++) {
    pinMode(PIR_PINS[i], INPUT);
    pinMode(LED_PINS[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  if (millis() - lastSimTime > 50) {
    simMinute++;
    if (simMinute >= 60) {
      simMinute = 0;
      simHour++;
      if (simHour > 23) simHour = 0;
    }
    lastSimTime = millis();
  }

  unsigned long currentSimulatedTime = simHour * 60 + simMinute;

  for (int i = 0; i < 4; i++) {
    int ldrValue = analogRead(LDR_PINS[i]);
    int pirValue = digitalRead(PIR_PINS[i]);
    int ledValue = offValue;

    // Update motion tracking
    if (pirValue == HIGH) {
      lastMotionSimulatedTime[i] = currentSimulatedTime;
      hasMotion[i] = true;
    }
    unsigned long sinceMotion = currentSimulatedTime - lastMotionSimulatedTime[i];

    // --- Main Logic ---
    if (simHour >= 23) {
      // After 11 PM: Force OFF
      ledValue = offValue;
    }
    else if (simHour >= 21) {
      // 9-11 PM: Dim by default, motion overrides
      if (hasMotion[i] && sinceMotion < dimAfter) {
        ledValue = fullBrightness;
      } else {
        ledValue = dimValue;
      }
    }
    else if (ldrValue > ldrBrightThreshold) {
      // Bright ambient light: OFF
      ledValue = offValue;
    }
    else {
      // Gradual dimming based on LDR (before 9 PM)
      if (ldrValue < ldrDarkThreshold) {
        // Map LDR value to brightness (darker = brighter LED)
        ledValue = map(ldrValue, 0, ldrDarkThreshold, fullBrightness, dimValue);
        ledValue = constrain(ledValue, dimValue, fullBrightness);
      }
      
      // Apply motion timeouts
      if (sinceMotion > offAfter) {
        ledValue = offValue;
      }
      else if (sinceMotion > dimAfter) {
        ledValue = dimValue;
      }
    }

    analogWrite(LED_PINS[i], ledValue);

    // Debug output
    Serial.print("Room "); Serial.print(i + 1);
    Serial.print(" | Time: "); Serial.print(simHour); Serial.print(":"); 
    if (simMinute < 10) Serial.print("0");
    Serial.print(simMinute);
    Serial.print(" | PIR: "); Serial.print(pirValue);  
    Serial.print(" | LDR: "); Serial.print(ldrValue);
    Serial.print(" | LED: "); Serial.println(ledValue);
  }

  // LCD Update
  int displayHour = simHour % 12;
  displayHour = displayHour ? displayHour : 12;
  char ampm[] = "AM";
  if (simHour >= 12) strcpy(ampm, "PM");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (displayHour < 10) lcd.print("0");
  lcd.print(displayHour);
  lcd.print(":");
  if (simMinute < 10) lcd.print("0");
  lcd.print(simMinute);
  lcd.print(" ");
  lcd.print(ampm);

  delay(200);
}