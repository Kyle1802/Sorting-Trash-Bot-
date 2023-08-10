#include <Servo.h>

Servo hand;
Servo wrist2;
Servo wrist1;
Servo shoulder;
Servo elbow;
Servo base;
Servo conveyor;

const int HOME_POSITION[] = {180, 40, 0, 25, 180, 90};
const int PICKUP_POSITION[] = {80, 180, 0, 25, 180, 90};
const int DROP_POSITIONS[][6] = {
  {80, 180, 0, 25, 150, 20},
  {80, 180, 0, 25, 150, 40},
  {80, 180, 0, 25, 150, 160},
  {80, 180, 0, 25, 150, 140}
};
const int NUM_DROP_POSITIONS = sizeof(DROP_POSITIONS) / sizeof(DROP_POSITIONS[0]);

bool isInitialized = false;
bool conveyorReset = false;
bool canMoveConveyor = false;  // Flag variable to track if conveyor movement is allowed

const int CONVEYOR_START_DELAY = 1600;  // Delay before starting the conveyor (in milliseconds)
const int CONVEYOR_STOP_DELAY = 1600;   // Delay before stopping the conveyor (in milliseconds)

void setup() {
  Serial.begin(9600);
  initializeServos();
}

void loop() {
  if (!isInitialized) {
    initializeArm();
    isInitialized = true;
  }

  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    executeAction(input);
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
