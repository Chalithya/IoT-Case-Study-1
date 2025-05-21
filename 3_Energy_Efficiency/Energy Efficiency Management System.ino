// ===== Pin Configuration =====
const int motionSensors[] = {2, 3, 4, 5};
const int lightIndicators[] = {10, 11, 12, 13};
const int fanIndicators[] = {6, 7, 8, 9};
const int relayControllers[] = {A0, A1, A2, A3};
const int heaterIndicators[] = {A4, A5};

// ===== Constants and States =====
const int totalRooms = 4;
const int totalHeaters = 2;
bool activeRooms[totalRooms] = {false};
bool activeHeaters[totalHeaters] = {false};
unsigned long lastActivity[totalRooms] = {0};
const unsigned long timeoutDuration = 600000; // 10 minutes in ms

bool linkHeater(int roomIndex, int heaterIndex) {
  return (heaterIndex == 0 && roomIndex < 2) || (heaterIndex == 1 && roomIndex >= 2);
}

void setup() {
  for (int i = 0; i < totalRooms; i++) {
    pinMode(motionSensors[i], INPUT_PULLUP);
    pinMode(relayControllers[i], OUTPUT);
    pinMode(lightIndicators[i], OUTPUT);
    pinMode(fanIndicators[i], OUTPUT);
  }

  for (int i = 0; i < totalHeaters; i++) {
    pinMode(heaterIndicators[i], OUTPUT);
  }

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Smart Room Automation Started");
}

void manageHeater(int heaterIndex) {
  bool turnOn = false;
  for (int i = 0; i < totalRooms; i++) {
    if (linkHeater(i, heaterIndex) && activeRooms[i]) {
      turnOn = true;
      break;
    }
  }

  if (activeHeaters[heaterIndex] != turnOn) {
    activeHeaters[heaterIndex] = turnOn;
    digitalWrite(heaterIndicators[heaterIndex], turnOn);
    Serial.print("[Heater #"); Serial.print(heaterIndex + 1);
    Serial.print("] Status changed to: ");
    Serial.println(turnOn ? "ACTIVE" : "INACTIVE");
  }
}

void displayRoomInfo(int roomIndex) {
  Serial.println("---------------------------");
  Serial.print("Room #"); Serial.println(roomIndex + 1);
  Serial.print("Motion: "); Serial.println(digitalRead(motionSensors[roomIndex]) ? "Yes" : "No");
  Serial.print("Relay: "); Serial.println(activeRooms[roomIndex] ? "ON" : "Off");
  Serial.print("Light: "); Serial.println(digitalRead(lightIndicators[roomIndex]) ? "ON" : "OFF");
  Serial.print("Fan: "); Serial.println(digitalRead(fanIndicators[roomIndex]) ? "ON" : "OFF");
  Serial.print("Last Movement: "); Serial.print((millis() - lastActivity[roomIndex]) / 1000);
  Serial.println(" seconds ago");
}

void activateRoom(int roomIndex, bool status) {
  if (activeRooms[roomIndex] != status) {
    activeRooms[roomIndex] = status;
    digitalWrite(relayControllers[roomIndex], !status);  // Inverted logic
    digitalWrite(lightIndicators[roomIndex], status);
    digitalWrite(fanIndicators[roomIndex], status);

    for (int i = 0; i < totalHeaters; i++) {
      if (linkHeater(roomIndex, i)) {
        manageHeater(i);
      }
    }

    displayRoomInfo(roomIndex);
  }
}

void loop() {
  for (int i = 0; i < totalRooms; i++) {
    if (digitalRead(motionSensors[i])) {
      lastActivity[i] = millis();
      if (!activeRooms[i]) {
        activateRoom(i, true);
      }
    } else if (activeRooms[i] && (millis() - lastActivity[i] > timeoutDuration)) {
      activateRoom(i, false);
    }
  }
}
