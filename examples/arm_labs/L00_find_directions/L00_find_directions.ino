// Lesson 00 - Find which way each joint turns on YOUR arm.
// Watch the arm and fill in the table in your lab notebook.
#include <BraccioV2.h>

Braccio arm;

void setup() {
  Serial.begin(9600);
  arm.begin();  // start upright
  Serial.println("Each joint will move +20 degrees, then back to centre.");
}

void loop() {
  Serial.println("BASE +20");
  arm.setOneRelative(BASE_ROT, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(BASE_ROT, -20);
  arm.safeDelay(1500);

  Serial.println("SHOULDER +20");
  arm.setOneRelative(SHOULDER, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(SHOULDER, -20);
  arm.safeDelay(1500);

  Serial.println("ELBOW +20");
  arm.setOneRelative(ELBOW, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(ELBOW, -20);
  arm.safeDelay(1500);

  Serial.println("WRIST +20");
  arm.setOneRelative(WRIST, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(WRIST, -20);
  arm.safeDelay(1500);

  Serial.println("WRIST_ROT +20");
  arm.setOneRelative(WRIST_ROT, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(WRIST_ROT, -20);
  arm.safeDelay(1500);

  Serial.println("GRIPPER +20 (closing)");
  arm.setOneRelative(GRIPPER, 20);
  arm.safeDelay(1500);
  arm.setOneRelative(GRIPPER, -20);
  arm.safeDelay(1500);

  Serial.println("Done. Pausing 5 s before repeating.");
  arm.safeDelay(5000);
}
