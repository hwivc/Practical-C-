#include "moves.h"

void moveToPose(Pose p, unsigned long waitMs) {
  arm.setAllAbsolute(p.base, p.shoulder, p.elbow, p.wrist, p.wristRot, p.gripper);
  arm.safeDelay(waitMs);
}

void printPose(Pose p) {
  Serial.print("Pose(");
  Serial.print(p.base);     Serial.print(", ");
  Serial.print(p.shoulder); Serial.print(", ");
  Serial.print(p.elbow);    Serial.print(", ");
  Serial.print(p.wrist);    Serial.print(", ");
  Serial.print(p.wristRot); Serial.print(", ");
  Serial.print(p.gripper);  Serial.println(")");
}
