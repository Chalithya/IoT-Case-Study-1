// Pin setup
const int potPin = A0;
const int pirPin = 3;
const int fanPin = 4;
const int lightPin = 5;
const int heaterPin = 6;
const int tempLedPin = 7;
const int motionLedPin = 8;

bool fanOn = false;
bool lightOn = false;
bool heaterOn = false;

void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(tempLedPin, OUTPUT);
  pinMode(motionLedPin, OUTPUT);

  Serial.println("=== Smart Home System ===");
  Serial.println("Commands:");
  Serial.println("F ON / OFF  => Fan");
  Serial.println("L ON / OFF  => Light");
  Serial.println("H ON / OFF  => Heater");
  Serial.println("STATUS       => Sensor readings");
  Serial.println("==========================");
}

void loop() {
  int tempRaw = analogRead(potPin);
  int tempC = map(tempRaw, 0, 1023, 0, 50);
  bool motion = digitalRead(pirPin);

  digitalWrite(fanPin, fanOn ? HIGH : LOW);
  digitalWrite(lightPin, lightOn ? HIGH : LOW);
  digitalWrite(heaterPin, heaterOn ? HIGH : LOW);

  digitalWrite(tempLedPin, tempC > 30 ? HIGH : LOW);
  digitalWrite(motionLedPin, motion ? HIGH : LOW);

  // Serial commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "F ON") {
      fanOn = true;
      Serial.println("Fan turned ON");
    } else if (cmd == "F OFF") {
      fanOn = false;
      Serial.println("Fan turned OFF");
    } else if (cmd == "L ON") {
      lightOn = true;
      Serial.println("Light turned ON");
    } else if (cmd == "L OFF") {
      lightOn = false;
      Serial.println("Light turned OFF");
    } else if (cmd == "H ON") {
      heaterOn = true;
      Serial.println("Heater turned ON");
    } else if (cmd == "H OFF") {
      heaterOn = false;
      Serial.println("Heater turned OFF");
    } else if (cmd == "STATUS") {
      printStatus(tempC, motion);
    } else {
      Serial.println("Unknown command");
    }
  }

  delay(300);
}

void printStatus(int temp, bool motion) {
  Serial.println("--- Sensor Status ---");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Motion Detected: ");
  Serial.println(motion ? "YES" : "NO");

  Serial.println("--- Device States ---");
  Serial.print("Fan: ");
  Serial.println(fanOn ? "ON" : "OFF");
  Serial.print("Light: ");
  Serial.println(lightOn ? "ON" : "OFF");
  Serial.print("Heater: ");
  Serial.println(heaterOn ? "ON" : "OFF");
  Serial.println();
}
