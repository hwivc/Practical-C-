// kinematics.h - forward and inverse kinematics for the Braccio (Project P5).
// Plain C++ with <math.h>: the same file compiles on a PC and on the Arduino.
#ifndef BRACCIO_KINEMATICS_H
#define BRACCIO_KINEMATICS_H

#include <math.h>
#include <stdlib.h>

namespace kin {

// Approximate link lengths in millimetres. Measure your own arm and adjust!
const double BASE_HEIGHT = 71.5;   // table to shoulder axis
const double L1 = 125.0;           // shoulder to elbow
const double L2 = 125.0;           // elbow to wrist
const double L3 = 195.0;           // wrist to gripper tip

const double PI_VALUE = 3.14159265358979323846;
inline double toRad(double deg) { return deg * PI_VALUE / 180.0; }
inline double toDeg(double rad) { return rad * 180.0 / PI_VALUE; }

// Servo angles for the four joints that position the gripper tip.
struct Angles {
  int base;
  int shoulder;
  int elbow;
  int wrist;
};

// Model conventions (see Project P1):
//   base 0 = pointing along +x, 90 = +y (straight ahead), 180 = -x
//   shoulder = angle of the upper arm above the horizontal (90 = straight up)
//   elbow, wrist: 90 = straight in line with the previous link
inline void forward(const Angles& a, double& x, double& y, double& z) {
  double a1 = toRad(a.shoulder);
  double a2 = a1 + toRad(a.elbow - 90);
  double a3 = a2 + toRad(a.wrist - 90);
  double reach = L1 * cos(a1) + L2 * cos(a2) + L3 * cos(a3);
  z = BASE_HEIGHT + L1 * sin(a1) + L2 * sin(a2) + L3 * sin(a3);
  x = reach * cos(toRad(a.base));
  y = reach * sin(toRad(a.base));
}

inline bool inRange(int v, int lo, int hi) { return v >= lo && v <= hi; }

// Inverse kinematics: gripper tip at (x, y, z) mm with the hand tilted `pitchDeg`
// (0 = horizontal, -90 = pointing straight down). Returns false if unreachable.
inline bool inverse(double x, double y, double z, double pitchDeg, Angles& out) {
  if (y < 0) return false;                        // the base only covers the front half

  // 1. Base: face the target (top view)
  double base = toDeg(atan2(y, x));
  double r = sqrt(x * x + y * y);                 // horizontal distance to the target

  // 2. Wrist position: step back from the tip along the hand
  double pitch = toRad(pitchDeg);
  double rw = r - L3 * cos(pitch);
  double zw = z - BASE_HEIGHT - L3 * sin(pitch);

  // 3. Two-link triangle (shoulder, elbow) with the law of cosines
  double c2 = (rw * rw + zw * zw - L1 * L1 - L2 * L2) / (2.0 * L1 * L2);
  if (c2 < -1.0 || c2 > 1.0) return false;        // too far or too close
  double t2 = -acos(c2);                          // negative = "elbow up" solution
  double t1 = atan2(zw, rw) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

  // 4. Convert link angles to servo angles
  double shoulder = toDeg(t1);
  double elbow = 90.0 + toDeg(t2);
  double wrist = 90.0 + toDeg(pitch - (t1 + t2));

  out.base = (int)lround(base);
  out.shoulder = (int)lround(shoulder);
  out.elbow = (int)lround(elbow);
  out.wrist = (int)lround(wrist);

  // 5. Respect the joint limits
  return inRange(out.base, 0, 180) && inRange(out.shoulder, 15, 165) &&
         inRange(out.elbow, 0, 180) && inRange(out.wrist, 0, 180);
}

// Try hand pitches from "pointing down" to slightly upward, and pick the most
// comfortable solution: the one whose joints stay closest to the middle of their range.
inline bool inverseAnyPitch(double x, double y, double z, Angles& out, int& usedPitch) {
  bool found = false;
  int bestCost = 0;
  for (int p = -90; p <= 30; p += 5) {
    Angles a;
    if (!inverse(x, y, z, p, a)) continue;
    int cost = abs(a.shoulder - 90) + abs(a.elbow - 90) + abs(a.wrist - 90);
    if (!found || cost < bestCost) {
      found = true;
      bestCost = cost;
      out = a;
      usedPitch = p;
    }
  }
  return found;
}

}  // namespace kin

#endif  // BRACCIO_KINEMATICS_H
