#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>

#define NUM_ROOMS 4

// PIR sensor pins
const int pirPins[NUM_ROOMS] = {2, 4, 7, 8};
// LDR sensor pins (analog)
const int ldrPins[NUM_ROOMS] = {A0, A1, A2, A3};
// Light control pins (PWM capable)
const int lightPins[NUM_ROOMS] = {3, 5, 6, 9};

// Thresholds for ambient light
const int LDR_THRESHOLD_LOW = 20;    
const int LDR_THRESHOLD_HIGH = 700;  

// Light brightness levels
const int LIGHT_ON = 255;
const int LIGHT_DIM = 20;    
const int LIGHT_OFF = 0;

const unsigned long FIVE_MINUTES = 5UL * 60UL * 1000UL;
const unsigned long FIFTEEN_MINUTES = 15UL * 60UL * 1000UL;
const unsigned long TEN_MINUTES = 10UL * 1000UL; // 10 seconds for diagnostics

unsigned long systemStartTime = 0;
unsigned long lastMotionTime[NUM_ROOMS] = {0};
bool motionDetected[NUM_ROOMS] = {false};

unsigned long lastPirChangeTime[NUM_ROOMS] = {0};
int lastPirState[NUM_ROOMS] = {-1};
unsigned long lastLdrChangeTime[NUM_ROOMS] = {0};
int lastLdrValue[NUM_ROOMS] = {-1};

bool pirFailed[NUM_ROOMS] = {false};
bool ldrFailed[NUM_ROOMS] = {false};

Adafruit_LiquidCrystal lcd(0);

int displayRoom = 0;
unsigned long lastDisplayChange = 0;
const unsigned long displayInterval = 3000; 

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.begin(16, 2);     
  lcd.setBacklight(1);  

  systemStartTime = millis();

  for (int i = 0; i < NUM_ROOMS; i++) {
    pinMode(pirPins[i], INPUT);
    pinMode(lightPins[i], OUTPUT);
    analogWrite(lightPins[i], LIGHT_OFF);

    
    lastPirChangeTime[i] = millis();
    lastLdrChangeTime[i] = millis();
    lastPirState[i] = digitalRead(pirPins[i]);
    lastLdrValue[i] = analogRead(ldrPins[i]);
    lastMotionTime[i] = millis();
  }
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - systemStartTime;

  int simulatedHour = (elapsedTime / 3600000UL) % 24;

  for (int i = 0; i < NUM_ROOMS; i++) {
    int pirState = digitalRead(pirPins[i]);
    int ldrValue = analogRead(ldrPins[i]);

    if (pirState != lastPirState[i]) {
      lastPirChangeTime[i] = currentMillis;
      lastPirState[i] = pirState;
      pirFailed[i] = false; 
    } else {
      if (currentMillis - lastPirChangeTime[i] > TEN_MINUTES) {
        pirFailed[i] = true;
      }
    }

   
    if (pirState == HIGH) {
      pirFailed[i] = false;        
      lastMotionTime[i] = currentMillis;
      motionDetected[i] = true;
    } else {
      motionDetected[i] = false;
    }

    
    bool ldrDisconnected = (ldrValue <= 10) || (ldrValue >= 1013);

    if (ldrDisconnected) {
      if (currentMillis - lastLdrChangeTime[i] > TEN_MINUTES) {
        ldrFailed[i] = true;
      }
    } else {
      if (abs(ldrValue - lastLdrValue[i]) > 10) {
        lastLdrChangeTime[i] = currentMillis;
        lastLdrValue[i] = ldrValue;
        ldrFailed[i] = false; 
      } else {
        if (currentMillis - lastLdrChangeTime[i] > TEN_MINUTES) {
          ldrFailed[i] = true;
        }
      }
    }

    unsigned long timeSinceLastMotion = currentMillis - lastMotionTime[i];
    int brightness = LIGHT_OFF;

    if (pirFailed[i] || ldrFailed[i]) {
      brightness = LIGHT_OFF;
    } else {

      if (ldrValue < LDR_THRESHOLD_LOW && pirState == HIGH) {
        brightness = LIGHT_ON;
      }
      else if (timeSinceLastMotion >= FIVE_MINUTES && timeSinceLastMotion < FIFTEEN_MINUTES) {
        brightness = LIGHT_DIM;
      }
      else if (ldrValue > LDR_THRESHOLD_HIGH || timeSinceLastMotion >= FIFTEEN_MINUTES) {
        brightness = LIGHT_OFF;
      }

      if (simulatedHour >= 21 && simulatedHour < 23) {
        brightness = LIGHT_DIM;
      }

      if (simulatedHour >= 23) {
        brightness = LIGHT_OFF;
      }
    }

    analogWrite(lightPins[i], brightness);

    Serial.print("Room ");
    Serial.print(i + 1);
    Serial.print(": PIR=");
    Serial.print(pirState);
    Serial.print(pirFailed[i] ? " FAIL" : "");
    Serial.print(", LDR=");
    Serial.print(ldrValue);
    Serial.print(ldrFailed[i] ? " FAIL" : "");
    Serial.print(", Brightness=");
    Serial.print(brightness);
    Serial.print(", SimHour=");
    Serial.println(simulatedHour);
  }

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (simulatedHour < 10) lcd.print('0');
  lcd.print(simulatedHour);
  lcd.print(":00     ");  

  if (currentMillis - lastDisplayChange > displayInterval) {
    lastDisplayChange = currentMillis;
    displayRoom = (displayRoom + 1) % NUM_ROOMS;

    lcd.setCursor(0, 1);

    bool pirUnchangedTooLong = (currentMillis - lastPirChangeTime[displayRoom] > TEN_MINUTES);
    bool ldrUnchangedTooLong = (currentMillis - lastLdrChangeTime[displayRoom] > TEN_MINUTES);

    if (pirUnchangedTooLong && ldrUnchangedTooLong) {
      lcd.print("R");
      lcd.print(displayRoom + 1);
      lcd.print(" Sensor FAIL ");
      lcd.print("   "); 
    } else {
      int pirState = digitalRead(pirPins[displayRoom]);
      int ldrValue = analogRead(ldrPins[displayRoom]);

      lcd.print("R");
      lcd.print(displayRoom + 1);
      lcd.print(":M");
      lcd.print(pirState ? "Y" : "N");
      lcd.print(" L");

      if (ldrValue < LDR_THRESHOLD_LOW) lcd.print("Lo ");
      else if (ldrValue > LDR_THRESHOLD_HIGH) lcd.print("Hi ");
      else lcd.print("Md ");

      lcd.print("    "); 
    }
  }

  delay(200);
}
