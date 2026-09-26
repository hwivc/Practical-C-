// Lesson 00 - Make the arm move
// Swings the base left and right while the rest of the arm stands upright.
#include <BraccioV2.h>

Braccio arm;  // one object that represents the whole robot arm

void setup() {
  Serial.begin(9600);
  Serial.println("Powering up the arm (about 8 seconds)...");
  arm.begin();  // soft-start the motors, then stand upright (all joints at centre)
  Serial.println("Ready!");
}

void loop() {
  //                base shoulder elbow wrist wristRot gripper
  arm.setAllAbsolute(45,   90,     90,   90,   90,      50);  // look right
  arm.safeDelay(2000);  // keep moving the joints for 2 seconds

  arm.setAllAbsolute(135,  90,     90,   90,   90,      50);  // look left
  arm.safeDelay(2000);
}
