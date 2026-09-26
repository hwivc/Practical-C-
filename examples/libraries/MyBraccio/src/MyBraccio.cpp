// MyBraccio.cpp - implementation. See MyBraccio.h for the public API.
#include "MyBraccio.h"

namespace {
// File-private constants (an unnamed namespace = visible only in this .cpp file).
const uint8_t PINS[MyBraccio::NUM_JOINTS]        = {11, 10, 9, 6, 5, 3};
const int     DEFAULT_MIN[MyBraccio::NUM_JOINTS] = {0, 15, 0, 0, 0, 10};
const int     DEFAULT_MAX[MyBraccio::NUM_JOINTS] = {180, 165, 180, 180, 180, 73};
const int     HOME_POSE[MyBraccio::NUM_JOINTS]   = {90, 90, 90, 90, 90, 50};
const uint8_t SOFT_START_PIN = 12;
}  // namespace

MyBraccio::MyBraccio() : _intervalMs(10), _lastStep(0), _started(false) {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    _min[j] = DEFAULT_MIN[j];
    _max[j] = DEFAULT_MAX[j];
    _current[j] = HOME_POSE[j];     // never 0: fixes BraccioV2 bug 5
    _target[j] = HOME_POSE[j];
    _speed[j] = 1;
  }
}

// ---------------------------------------------------------------- start-up
void MyBraccio::begin() {
  begin(HOME_POSE);
}

void MyBraccio::begin(const int startPose[NUM_JOINTS]) {
  pinMode(SOFT_START_PIN, OUTPUT);
  digitalWrite(SOFT_START_PIN, LOW);            // motors off while we prepare
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    int a = _clamp(j, startPose[j]);
    _current[j] = a;
    _target[j] = a;
    _servos[j].attach(PINS[j]);
    _servos[j].write(a);                        // command the pose BEFORE power arrives
  }
  _softStart();
  _lastStep = millis();
  _started = true;
}

// Same power-up profile as the Arduino Braccio library and BraccioV2 (shield V4+).
void MyBraccio::_softStart() {
  unsigned long start = millis();
  while (millis() - start < 2000UL) {
    digitalWrite(SOFT_START_PIN, HIGH); delayMicroseconds(80);
    digitalWrite(SOFT_START_PIN, LOW);  delayMicroseconds(450);
  }
  while (millis() - start < 6000UL) {
    digitalWrite(SOFT_START_PIN, HIGH); delayMicroseconds(75);
    digitalWrite(SOFT_START_PIN, LOW);  delayMicroseconds(430);
  }
  digitalWrite(SOFT_START_PIN, HIGH);
}

// ---------------------------------------------------------------- commands
int MyBraccio::_clamp(uint8_t joint, int angle) const {
  if (angle < _min[joint]) return _min[joint];
  if (angle > _max[joint]) return _max[joint];
  return angle;
}

bool MyBraccio::setTarget(uint8_t joint, int angle) {
  if (!_valid(joint)) return false;             // fixes bug 4
  int safe = _clamp(joint, angle);
  _target[joint] = safe;
  return safe == angle;                         // fixes bug 1
}

bool MyBraccio::setAll(int base, int shoulder, int elbow, int wrist, int wristRot, int gripper) {
  const int pose[NUM_JOINTS] = {base, shoulder, elbow, wrist, wristRot, gripper};
  return setAll(pose);
}

bool MyBraccio::setAll(const int pose[NUM_JOINTS]) {
  bool allExact = true;
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    allExact = setTarget(j, pose[j]) && allExact;   // setTarget runs for EVERY joint
  }
  return allExact;
}

bool MyBraccio::nudge(uint8_t joint, int degrees) {
  if (!_valid(joint)) return false;
  return setTarget(joint, _target[joint] + degrees);
}

void MyBraccio::stop() {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) _target[j] = _current[j];
}

// ---------------------------------------------------------------- configuration
bool MyBraccio::setLimits(uint8_t joint, int minAngle, int maxAngle) {
  if (!_valid(joint)) return false;
  if (minAngle < 0 || maxAngle > 180 || minAngle > maxAngle) return false;   // reject (Lesson 10)
  _min[joint] = minAngle;
  _max[joint] = maxAngle;
  _target[joint] = _clamp(joint, _target[joint]);   // keep the invariant min <= target <= max
  return true;
}

bool MyBraccio::setSpeed(uint8_t joint, uint8_t degreesPerStep) {
  if (!_valid(joint) || degreesPerStep < 1 || degreesPerStep > 10) return false;   // fixes bug 6
  _speed[joint] = degreesPerStep;
  return true;
}

void MyBraccio::setSpeedAll(uint8_t degreesPerStep) {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) setSpeed(j, degreesPerStep);
}

bool MyBraccio::setStepInterval(uint16_t ms) {
  if (ms < 5 || ms > 100) return false;
  _intervalMs = ms;
  return true;
}

// ---------------------------------------------------------------- motion
void MyBraccio::_write(uint8_t joint, int angle) {
  _current[joint] = angle;
  _servos[joint].write(angle);
}

void MyBraccio::_step(uint8_t joint) {
  int remaining = _target[joint] - _current[joint];
  if (remaining == 0) return;
  int s = _speed[joint];
  int step = remaining > s ? s : (remaining < -s ? -s : remaining);   // never overshoot: fixes bug 2
  _write(joint, _current[joint] + step);
}

void MyBraccio::update() {
  if (!_started) return;                        // servos not attached yet
  unsigned long now = millis();
  if (now - _lastStep < _intervalMs) return;    // not due yet: return immediately
  _lastStep = now;                              // durations, not end times: fixes bug 3
  for (uint8_t j = 0; j < NUM_JOINTS; j++) _step(j);
}

void MyBraccio::waitUntilStopped(unsigned long timeoutMs) {
  unsigned long start = millis();
  while (isMoving() && millis() - start < timeoutMs) {
    update();
  }
}

void MyBraccio::delayWhileMoving(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    update();
  }
}

// ---------------------------------------------------------------- queries
int MyBraccio::angle(uint8_t joint) const    { return _valid(joint) ? _current[joint] : -1; }
int MyBraccio::target(uint8_t joint) const   { return _valid(joint) ? _target[joint] : -1; }
int MyBraccio::minAngle(uint8_t joint) const { return _valid(joint) ? _min[joint] : -1; }
int MyBraccio::maxAngle(uint8_t joint) const { return _valid(joint) ? _max[joint] : -1; }

bool MyBraccio::isMoving(uint8_t joint) const {
  return _valid(joint) && _current[joint] != _target[joint];
}

bool MyBraccio::isMoving() const {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    if (_current[j] != _target[j]) return true;
  }
  return false;
}
