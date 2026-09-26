// Lesson 06 - pose.h is included TWICE here (directly, and again through moves.h).
// Without include guards, struct Pose would be defined twice and the build would fail.
#include "pose.h"
#include "moves.h"

Braccio arm;

const Pose REACH = {90, 60, 60, 60, 90, 10};

void setup() {
  Serial.begin(9600);
  arm.begin();
}

void loop() {
  printPose(REACH);
  moveToPose(REACH, 2000);
  printPose(HOME);
  moveToPose(HOME, 2000);
}
