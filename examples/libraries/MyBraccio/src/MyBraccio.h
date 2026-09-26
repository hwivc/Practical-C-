// MyBraccio.h - a safe, non-blocking driver for the Tinkerkit Braccio arm.
// Written step by step in Lesson 19 of "Practical C++ with the Braccio Arm".
// Design inspired by BraccioV2 (Lukas Severinghaus) and the Arduino Braccio library.
#ifndef MY_BRACCIO_H
#define MY_BRACCIO_H

#include <Arduino.h>
#include <Servo.h>

class MyBraccio {
public:
  // Joint names live INSIDE the class: MyBraccio::ELBOW. No macros, no clashes.
  enum Joint : uint8_t { BASE = 0, SHOULDER, ELBOW, WRIST, WRIST_ROT, GRIPPER };
  static const uint8_t NUM_JOINTS = 6;

  MyBraccio();   // stores defaults only: no hardware access (Lesson 09)

  // ---- Start-up ---------------------------------------------------------
  void begin();                                  // soft-start, then hold the home pose
  void begin(const int startPose[NUM_JOINTS]);   // soft-start, then hold startPose

  // ---- Commands (set targets; motion happens in update()) ---------------
  bool setTarget(uint8_t joint, int angle);      // true if the angle was used unchanged
  bool setAll(int base, int shoulder, int elbow, int wrist, int wristRot, int gripper);
  bool setAll(const int pose[NUM_JOINTS]);
  bool nudge(uint8_t joint, int degrees);        // relative to the current target
  void stop();                                   // freeze every joint where it is now

  // ---- Configuration ----------------------------------------------------
  bool setLimits(uint8_t joint, int minAngle, int maxAngle);   // rejects bad input
  bool setSpeed(uint8_t joint, uint8_t degreesPerStep);        // 1..10
  void setSpeedAll(uint8_t degreesPerStep);
  bool setStepInterval(uint16_t ms);                           // 5..100 ms between steps

  // ---- Motion -----------------------------------------------------------
  void update();                                 // call as often as you like; steps when due
  void waitUntilStopped(unsigned long timeoutMs = 10000);
  void delayWhileMoving(unsigned long ms);       // like BraccioV2's safeDelay, but correct

  // ---- Queries (const: they never change the arm) -----------------------
  int angle(uint8_t joint) const;
  int target(uint8_t joint) const;
  int minAngle(uint8_t joint) const;
  int maxAngle(uint8_t joint) const;
  bool isMoving() const;
  bool isMoving(uint8_t joint) const;
  bool started() const { return _started; }

private:
  static bool _valid(uint8_t joint) { return joint < NUM_JOINTS; }
  int _clamp(uint8_t joint, int angle) const;
  void _write(uint8_t joint, int angle);
  void _step(uint8_t joint);
  void _softStart();

  Servo _servos[NUM_JOINTS];
  int _min[NUM_JOINTS];
  int _max[NUM_JOINTS];
  int _current[NUM_JOINTS];
  int _target[NUM_JOINTS];
  uint8_t _speed[NUM_JOINTS];
  uint16_t _intervalMs;
  unsigned long _lastStep;
  bool _started;
};

#endif  // MY_BRACCIO_H
