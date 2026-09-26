// Lesson 04 - Functions: give every arm action a name.
#include <BraccioV2.h>

Braccio arm;

const int GRIPPER_OPEN = 10;
const int GRIPPER_CLOSED = 73;

// ---- Small, single-purpose functions ------------------------------------

// Move all six joints and wait long enough to arrive.
void moveTo(int base, int shoulder, int elbow, int wrist, int wristRot, int gripper,
            unsigned long waitMs = 1500) {
  arm.setAllAbsolute(base, shoulder, elbow, wrist, wristRot, gripper);
  arm.safeDelay(waitMs);
}

void openGripper() {
  arm.setOneAbsolute(GRIPPER, GRIPPER_OPEN);
  arm.safeDelay(600);
}

void closeGripper() {
  arm.setOneAbsolute(GRIPPER, GRIPPER_CLOSED);
  arm.safeDelay(600);
}

// Returns how far (in degrees) the base will travel between two angles.
int travel(int from, int to) {
  int distance = to - from;
  if (distance < 0) {
    distance = -distance;
  }
  return distance;
}

// Wave the wrist `times` times. A static local remembers how often we've waved.
void wave(int times) {
  static int totalWaves = 0;
  for (int i = 0; i < times; i++) {
    arm.setOneAbsolute(WRIST, 60);
    arm.safeDelay(500);
    arm.setOneAbsolute(WRIST, 120);
    arm.safeDelay(500);
    totalWaves++;
  }
  arm.setOneAbsolute(WRIST, 90);
  arm.safeDelay(500);
  Serial.print("Total waves since power-on: ");
  Serial.println(totalWaves);
}

// ---- The program reads like a story now ---------------------------------

void setup() {
  Serial.begin(9600);
  arm.begin();
  wave(3);
}

void loop() {
  Serial.print("Base will travel ");
  Serial.print(travel(60, 120));
  Serial.println(" degrees");

  moveTo(60, 80, 80, 80, 90, GRIPPER_OPEN);   // over the pick-up spot
  closeGripper();                            // grab
  moveTo(120, 90, 90, 90, 90, GRIPPER_CLOSED, 2000);  // carry (custom wait)
  openGripper();                             // release
  wave(1);
}
