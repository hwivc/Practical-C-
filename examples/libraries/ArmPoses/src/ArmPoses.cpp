#include "ArmPoses.h"

PosePlayer::PosePlayer(Braccio& arm) : _arm(arm), _current(Poses::HOME) {}

int PosePlayer::_lerp(int a, int b, int step, int steps) {
  return a + (long)(b - a) * step / steps;
}

void PosePlayer::moveTo(const Pose& target, unsigned long durationMs) {
  const int STEPS = 40;
  const Pose start = _current;
  for (int i = 1; i <= STEPS; i++) {
    _current.base     = _lerp(start.base,     target.base,     i, STEPS);
    _current.shoulder = _lerp(start.shoulder, target.shoulder, i, STEPS);
    _current.elbow    = _lerp(start.elbow,    target.elbow,    i, STEPS);
    _current.wrist    = _lerp(start.wrist,    target.wrist,    i, STEPS);
    _current.wristRot = _lerp(start.wristRot, target.wristRot, i, STEPS);
    _current.gripper  = _lerp(start.gripper,  target.gripper,  i, STEPS);
    _arm.setAllAbsolute(_current.base, _current.shoulder, _current.elbow,
                        _current.wrist, _current.wristRot, _current.gripper);
    _arm.safeDelay(durationMs / STEPS);
  }
}

void PosePlayer::play(const Pose sequence[], int count, unsigned long durationMs) {
  for (int i = 0; i < count; i++) {
    moveTo(sequence[i], durationMs);
  }
}
