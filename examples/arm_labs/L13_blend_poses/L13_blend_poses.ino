// Lesson 13 - References: blend between two poses so ALL joints arrive together.
#include <BraccioV2.h>

Braccio arm;

struct Pose {
  int base, shoulder, elbow, wrist, wristRot, gripper;
};

const Pose HOME  = {90, 90, 90, 90, 90, 50};
const Pose LEFT  = {150, 70, 110, 60, 90, 10};
const Pose RIGHT = {30, 100, 80, 120, 150, 73};

// Linear interpolation between a and b. percent: 0 -> a, 100 -> b.
int lerp(int a, int b, int percent) {
  return a + (long)(b - a) * percent / 100;
}

// Inputs by const reference (no copies, read-only); output by reference (written).
void blend(const Pose& from, const Pose& to, int percent, Pose& out) {
  out.base     = lerp(from.base,     to.base,     percent);
  out.shoulder = lerp(from.shoulder, to.shoulder, percent);
  out.elbow    = lerp(from.elbow,    to.elbow,    percent);
  out.wrist    = lerp(from.wrist,    to.wrist,    percent);
  out.wristRot = lerp(from.wristRot, to.wristRot, percent);
  out.gripper  = lerp(from.gripper,  to.gripper,  percent);
}

// Moves from one pose to another in `durationMs`, every joint finishing at the same moment.
// 'current' is updated so the caller always knows where the arm is.
void glide(Pose& current, const Pose& target, unsigned long durationMs) {
  const Pose start = current;          // a copy: the starting point must not change
  const int STEPS = 50;
  for (int i = 1; i <= STEPS; i++) {
    blend(start, target, i * 100 / STEPS, current);
    arm.setAllAbsolute(current.base, current.shoulder, current.elbow,
                       current.wrist, current.wristRot, current.gripper);
    arm.safeDelay(durationMs / STEPS);
  }
}

Pose where = HOME;   // where the arm is now

void setup() {
  Serial.begin(9600);
  arm.begin();
}

void loop() {
  glide(where, LEFT, 2000);
  Serial.print("At LEFT, base = ");
  Serial.println(where.base);
  glide(where, RIGHT, 3000);
  Serial.print("At RIGHT, base = ");
  Serial.println(where.base);
  glide(where, HOME, 1500);
}
