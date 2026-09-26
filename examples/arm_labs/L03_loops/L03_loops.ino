// Lesson 03 - Loops: test every joint with a for loop, then do a smooth sweep.
#include <BraccioV2.h>

Braccio arm;

const int NUM_JOINTS = 6;
const int TEST_OFFSET = 20;

void setup() {
  Serial.begin(9600);
  arm.begin();

  // The whole of Lesson 00's "find directions" sketch in one loop:
  for (int joint = 0; joint < NUM_JOINTS; joint++) {
    Serial.print("Testing joint ");
    Serial.println(joint);
    arm.setOneRelative(joint, TEST_OFFSET);
    arm.safeDelay(1200);
    arm.setOneRelative(joint, -TEST_OFFSET);
    arm.safeDelay(1200);
  }
  Serial.println("Joint test complete.");
}

void loop() {
  // Sweep the base from 30 to 150 in steps of 30, pausing at each stop.
  for (int angle = 30; angle <= 150; angle += 30) {
    arm.setOneAbsolute(BASE_ROT, angle);
    arm.safeDelay(700);

    // Close the gripper only at the middle stop
    if (angle == 90) {
      arm.setOneAbsolute(GRIPPER, 73);
    } else {
      arm.setOneAbsolute(GRIPPER, 30);
    }
  }

  // Come back with a while loop, in bigger steps
  int angle = 150;
  while (angle > 30) {
    angle -= 60;
    arm.setOneAbsolute(BASE_ROT, angle);
    arm.safeDelay(900);
  }
}
