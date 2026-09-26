// Project P5 - Reach a point: type "x y z" (millimetres) and the gripper goes there.
// Serial Monitor, line ending: Newline.  Example: 0 200 100
// Needs the MyBraccio library (Lesson 19). kinematics.h is in this sketch's folder.
#include <MyBraccio.h>
#include <stdlib.h>
#include <string.h>
#include "kinematics.h"

MyBraccio arm;

// If a joint turns the "wrong" way on your arm compared with the model
// (Lesson 00's direction table), set its flag to true.
const bool MIRROR_SHOULDER = false;
const bool MIRROR_ELBOW = false;
const bool MIRROR_WRIST = false;

int toServo(int modelAngle, bool mirrored) {
  return mirrored ? 180 - modelAngle : modelAngle;
}

// ---- Line input (as in Project P3) -----------------------------------------
const uint8_t MAX_LINE = 32;
char line[MAX_LINE];
uint8_t lineLength = 0;

bool readLine() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      line[lineLength] = '\0';
      lineLength = 0;
      return true;
    }
    if (lineLength < MAX_LINE - 1) line[lineLength++] = c;
  }
  return false;
}

bool parseNumber(const char* token, long& out) {
  if (token == nullptr) return false;
  char* end;
  out = strtol(token, &end, 10);
  return *end == '\0' && end != token;
}

void reach(long x, long y, long z) {
  kin::Angles a;
  int pitch;
  if (!kin::inverseAnyPitch(x, y, z, a, pitch)) {
    Serial.println(F("Unreachable (too far, too close, behind the arm, or outside a joint's limits)"));
    return;
  }
  Serial.print(F("pitch "));
  Serial.print(pitch);
  Serial.print(F(" -> base "));
  Serial.print(a.base);
  Serial.print(F(", shoulder "));
  Serial.print(a.shoulder);
  Serial.print(F(", elbow "));
  Serial.print(a.elbow);
  Serial.print(F(", wrist "));
  Serial.println(a.wrist);

  arm.setTarget(MyBraccio::BASE, a.base);
  arm.setTarget(MyBraccio::SHOULDER, toServo(a.shoulder, MIRROR_SHOULDER));
  arm.setTarget(MyBraccio::ELBOW, toServo(a.elbow, MIRROR_ELBOW));
  arm.setTarget(MyBraccio::WRIST, toServo(a.wrist, MIRROR_WRIST));
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  arm.setSpeedAll(2);
  Serial.println(F("Type a target as: x y z   (mm; y is straight ahead, z is up)"));
  Serial.println(F("Try: 0 200 100"));
}

void loop() {
  arm.update();
  if (!readLine()) return;

  long x, y, z;
  char* tx = strtok(line, " ,");
  char* ty = strtok(nullptr, " ,");
  char* tz = strtok(nullptr, " ,");
  if (!parseNumber(tx, x) || !parseNumber(ty, y) || !parseNumber(tz, z)) {
    Serial.println(F("ERR expected three numbers, e.g. 0 200 100"));
    return;
  }
  Serial.print(F("> target "));
  Serial.print(x); Serial.print(' ');
  Serial.print(y); Serial.print(' ');
  Serial.println(z);
  reach(x, y, z);
}
