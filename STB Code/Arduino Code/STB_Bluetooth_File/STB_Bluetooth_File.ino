#include <Servo.h>
#include <NewPing.h>
#include <SoftwareSerial.h> // Include the SoftwareSerial library

Servo hand;
Servo wrist2;
Servo wrist1;
Servo shoulder;
Servo elbow;
Servo base;
Servo conveyor;

const int HOME_POSITION[] = {180, 40, 0, 25, 180, 90};
const int PICKUP_POSITION[] = {80, 180, 0, 18, 180, 90};
const int DROP_POSITIONS[][6] = {
  {80, 180, 0, 25, 150, 20},
  {80, 180, 0, 25, 150, 40},
  {80, 180, 0, 25, 150, 160},
  {80, 180, 0, 25, 150, 140}
};
const int NUM_DROP_POSITIONS = sizeof(DROP_POSITIONS) / sizeof(DROP_POSITIONS[0]);

bool isInitialized = false;
bool canMoveConveyor = false;

const int CONVEYOR_START_DELAY = 1600;
const int CONVEYOR_STOP_DELAY = 1600;

const int TRIGGER_PIN = 3;
const int ECHO_PIN = 4;
const int MAX_DISTANCE = 400;

NewPing ultrasonicSensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

SoftwareSerial bluetoothSerial(11, 10); // Create a SoftwareSerial object for Bluetooth

void setup() {
  Serial.begin(9600);
  bluetoothSerial.begin(9600); // Initialize Bluetooth communication
  initializeServos();
}

void loop() {
  if (!isInitialized) {
    initializeArm();
    isInitialized = true;
  }

  if (bluetoothSerial.available() > 0) { // Check for Bluetooth input
    int input = bluetoothSerial.parseInt(); // Read integer input from Bluetooth
    if (input >= 1 && input <= 4) {
      executeAction(input);
    } else {
      bluetoothSerial.println("Invalid input. Please enter 1, 2, 3, or 4.");
    }
  }
}

void initializeServos() {
  hand.attach(2);
  wrist1.attach(6);
  wrist2.attach(5);
  shoulder.attach(10);
  elbow.attach(9);
  base.attach(12);
  conveyor.attach(8);
  
  // Set the conveyor to the stopped position
  conveyor.write(0);
}

void initializeArm() {
  moveServos(HOME_POSITION);
}

void executeAction(int action) {
  if (action >= 1 && action <= 4) {
    canMoveConveyor = true;  // Allow conveyor movement when a valid action is entered
    
    int dropIndex = action - 1;
    moveConveyor(100);
    delay(CONVEYOR_START_DELAY);  // Adjust the delay before starting the conveyor
    
    
    moveConveyor(0);
    delay(CONVEYOR_STOP_DELAY);   // Adjust the delay before stopping the conveyor
    
    moveServos(PICKUP_POSITION);
    delay(500);
    moveServos(DROP_POSITIONS[dropIndex]);
    moveServos(HOME_POSITION);
    
    canMoveConveyor = false;  // Prevent further conveyor movement until a new valid action is entered
  } else {
    Serial.println("Invalid input");
  }
}

void moveServos(const int angles[]) {
  int currentAngles[] = {hand.read(), wrist2.read(), wrist1.read(), shoulder.read(), elbow.read(), base.read()};
  const int speedDelay = 12;

  for (int i = 0; i < 6; i++) {
    int currentAngle = currentAngles[i];
    int targetAngle = angles[i];

    for (int angle = currentAngle; angle != targetAngle; angle += (targetAngle > currentAngle) ? 1 : -1) {
      setServoAngle(i, angle);
      delay(speedDelay);
    }
  }
}

void moveConveyor(int angle) {
  conveyor.write(angle);
  delay(200);
}

void setServoAngle(int servoIndex, int angle) {
  switch (servoIndex) {
    case 0:
      hand.write(angle);
      break;
    case 1:
      wrist2.write(angle);
      break;
    case 2:
      wrist1.write(angle);
      break;
    case 3:
      shoulder.write(angle);
      break;
    case 4:
      elbow.write(angle);
      break;
    case 5:
      base.write(angle);
      break;
    default:
      break;
  }
}
