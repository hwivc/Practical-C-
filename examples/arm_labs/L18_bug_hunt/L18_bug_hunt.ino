// Lesson 18 - Watch two BraccioV2 bugs happen on real hardware (safely).
#include <BraccioV2.h>

Braccio arm;

void printResult(const char* call, bool returned, bool expected) {
  Serial.print(call);
  Serial.print(" returned ");
  Serial.print(returned ? "true " : "false");
  Serial.print("  (should be ");
  Serial.print(expected ? "true" : "false");
  Serial.println(returned == expected ? ")  OK" : ")  <-- BUG");
}

void setup() {
  Serial.begin(9600);
  arm.begin();

  Serial.println(F("--- Bug 1: setOneAbsolute's return value ---"));
  // 90 is inside the elbow's range, so nothing is clamped: expected true.
  printResult("setOneAbsolute(ELBOW, 90)  ", arm.setOneAbsolute(ELBOW, 90), true);
  // -40 is clamped to 0, so expected false... but BASE_ROT is joint 0.
  printResult("setOneAbsolute(BASE_ROT,-40)", arm.setOneAbsolute(BASE_ROT, -40), false);
  arm.setOneAbsolute(BASE_ROT, 90);   // put the target back before the arm moves
  // 200 is clamped to 180: expected false (correct only by accident, 2 != 180).
  printResult("setOneAbsolute(ELBOW, 200) ", arm.setOneAbsolute(ELBOW, 200), false);
  arm.setOneAbsolute(ELBOW, 90);
  arm.safeDelay(2000);

  Serial.println(F("--- Bug 2: delta overshoot ---"));
  Serial.println(F("Base target 90 -> 100 with delta 7: 90, 97, 104, 97, 104, ..."));
  Serial.println(F("Watch the base: it jitters forever instead of stopping at 100."));
  arm.setDelta(BASE_ROT, 7);
  arm.setOneAbsolute(BASE_ROT, 100);
  arm.safeDelay(4000);

  Serial.println(F("Fix: use a delta that divides the distance, or fix _moveServo (Lesson 19)."));
  arm.setDelta(BASE_ROT, 1);
  arm.setOneAbsolute(BASE_ROT, 90);
  arm.safeDelay(1500);
}

void loop() {
}
