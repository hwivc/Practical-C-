// Lesson 07 - Conditional compilation: a debug switch and a "dry run" safety mode.
#include <BraccioV2.h>

// ---- Build switches: comment a line out to turn that feature off --------
#define DEBUG      // print every command to the Serial Monitor
// #define DRY_RUN // print commands but DON'T move the arm (test risky code safely)

#ifdef DEBUG
  #define DEBUG_PRINT(x)   Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  // In a release build these vanish completely: zero bytes, zero time.
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

Braccio arm;

void moveJoint(int joint, int angle) {
  DEBUG_PRINT("[line ");
  DEBUG_PRINT(__LINE__);
  DEBUG_PRINT("] joint ");
  DEBUG_PRINT(joint);
  DEBUG_PRINT(" -> ");
  DEBUG_PRINTLN(angle);

#ifndef DRY_RUN
  arm.setOneAbsolute(joint, angle);
  arm.safeDelay(800);
#endif
}

void setup() {
  Serial.begin(9600);
#ifdef DRY_RUN
  Serial.println("DRY RUN: the arm will not move.");
#else
  arm.begin();
#endif
  DEBUG_PRINTLN("Setup finished");
}

void loop() {
  moveJoint(BASE_ROT, 60);
  moveJoint(ELBOW, 120);
  moveJoint(BASE_ROT, 120);
  moveJoint(ELBOW, 90);
}
