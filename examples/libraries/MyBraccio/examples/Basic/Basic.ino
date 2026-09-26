// MyBraccio - Basic example: move between two poses.
#include <MyBraccio.h>

MyBraccio arm;

void setup() {
  Serial.begin(9600);
  arm.setLimits(MyBraccio::GRIPPER, 10, 65);   // configuration can happen before begin()
  arm.begin();                                 // ~6 s soft-start, then hold the home pose
  arm.setSpeed(MyBraccio::BASE, 2);            // the base moves 2 degrees per step
}

void loop() {
  if (!arm.setAll(40, 80, 100, 70, 90, 73)) {
    Serial.println("Some angles were clamped to the limits");   // gripper 73 > 65
  }
  arm.waitUntilStopped();
  arm.delayWhileMoving(500);

  arm.setAll(140, 90, 90, 90, 90, 10);
  arm.waitUntilStopped();
  arm.delayWhileMoving(500);
}
