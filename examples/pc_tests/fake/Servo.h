// fake/Servo.h - records what would have been sent to each servo.
#ifndef FAKE_SERVO_H
#define FAKE_SERVO_H

#include "Arduino.h"

namespace fake {
extern int servoAngle[20];     // last angle written, by pin (-1 = never written)
extern int servoWrites;        // total number of write() calls
}

class Servo {
public:
  uint8_t attach(int pin) { _pin = pin; return 1; }
  void write(int angle) {
    if (_pin >= 0) fake::servoAngle[_pin] = angle;
    fake::servoWrites++;
  }
private:
  int _pin = -1;
};

#endif
