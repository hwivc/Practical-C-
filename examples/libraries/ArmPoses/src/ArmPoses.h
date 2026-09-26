// ArmPoses.h - named poses and smooth playback for the Braccio (Lesson 17).
#ifndef ARM_POSES_H
#define ARM_POSES_H

#include <Arduino.h>
#include <BraccioV2.h>

// All six joint angles of one arm pose.
struct Pose {
  int base;
  int shoulder;
  int elbow;
  int wrist;
  int wristRot;
  int gripper;
};

// A namespace groups our names so they can't clash with names in other libraries.
namespace Poses {
  const Pose HOME = {90, 90, 90, 90, 90, 50};
  const Pose PARK = {90, 45, 180, 180, 90, 10};
}

class PosePlayer {
public:
  explicit PosePlayer(Braccio& arm);

  // Glide from the current pose to `target` in `durationMs`, all joints arriving together.
  void moveTo(const Pose& target, unsigned long durationMs);

  // Play `count` poses in order, spending `durationMs` on each.
  void play(const Pose sequence[], int count, unsigned long durationMs);

  const Pose& current() const { return _current; }

private:
  static int _lerp(int a, int b, int step, int steps);

  Braccio& _arm;
  Pose _current;
};

#endif  // ARM_POSES_H
