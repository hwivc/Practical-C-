// fake/Arduino.h - just enough of the Arduino API to compile MyBraccio on a PC.
// The clock is controlled by the test, so tests are fast and repeatable.
#ifndef FAKE_ARDUINO_H
#define FAKE_ARDUINO_H

#include <cstdint>

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define LED_BUILTIN 13

namespace fake {
extern unsigned long nowMs;          // the fake clock, in milliseconds
extern int pinLevel[20];             // last value written to each pin
}

inline unsigned long millis() { return fake::nowMs; }
inline void delay(unsigned long ms) { fake::nowMs += ms; }
inline void delayMicroseconds(unsigned int us) { fake::nowMs += us / 1000 + (us % 1000 ? 1 : 0); }
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t pin, uint8_t v) { fake::pinLevel[pin] = v; }

#endif
