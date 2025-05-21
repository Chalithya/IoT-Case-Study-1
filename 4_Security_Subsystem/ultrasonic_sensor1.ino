#include <Keypad.h>

// Keypad Setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Row pin numbers in Arduino
byte colPins[COLS] = {5, 4, 3, 2}; // Column pin numbers in Arduino
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// HC-SR04 Ultrasonic Sensor
const int trigPin = 10;
const int echoPin = 11;

// Alarm Outputs
const int buzzerPin = 12;
const int ledPin = 13;

// Security Keys Variable (Passcode)
String inputCode = "";
const String correctCode = "1234";
bool isArmed = false;
bool alarmOn = false;

void setup() {
  Serial.begin(9600);
  Serial.println("\nAssignment 1 || Question No. 4: Security Subsystem \n");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
  Serial.println("System is ready. Enter 1234# to Arm/Disarm.");
}

void loop() {
  // Listen for keypad input
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '#') {
      Serial.print("Entered Code: ");
      Serial.println(inputCode);

      if (inputCode == correctCode) {
        isArmed = !isArmed;
        alarmOn = false;
        Serial.println(isArmed ? "Alarm Armed" : "Alarm Disarmed");
        digitalWrite(buzzerPin, LOW);
        digitalWrite(ledPin, LOW);
      } else {
        Serial.println("Incorrect Code - Unauthorized Access Detected");
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(ledPin, HIGH);
        delay(2000); // Unauthorized access alert
        digitalWrite(buzzerPin, LOW);
        digitalWrite(ledPin, LOW);
      }
      inputCode = "";
    } else if (key == '*') {
      inputCode = "";
      Serial.println("Code Cleared");
    } else {
      inputCode += key;
    }
  }

  // Arm/Disarm
  if (isArmed) {
    long duration, distance;

    // Trigger ultrasonic pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read echo time
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > 0 && distance < 50) {
      if (!alarmOn) {
        Serial.println("Motion Detected! Alarm Triggered."); // When distance is less than 50, Alarm is triggered
        alarmOn = true;
      }
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
    } else {
      alarmOn = false;
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
    }
  }

  delay(100);
}
