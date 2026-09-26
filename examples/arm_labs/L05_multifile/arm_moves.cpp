// arm_moves.cpp - DEFINITIONS of the arm actions (the "kitchen").
#include "arm_moves.h"

void goHome() {
  arm.setAllAbsolute(90, 90, 90, 90, 90, 50);
  arm.safeDelay(1500);
}

void openGripper() {
  arm.setOneAbsolute(GRIPPER, GRIPPER_OPEN);
  arm.safeDelay(600);
}

void closeGripper() {
  arm.setOneAbsolute(GRIPPER, GRIPPER_CLOSED);
  arm.safeDelay(600);
}

void wave(int times) {
  for (int i = 0; i < times; i++) {
    arm.setOneAbsolute(WRIST, 60);
    arm.safeDelay(500);
    arm.setOneAbsolute(WRIST, 120);
    arm.safeDelay(500);
  }
  arm.setOneAbsolute(WRIST, 90);
  arm.safeDelay(500);
}
