// Lesson 16 - Measure what the build system produced: how fast is arm.update()?
#include <BraccioV2.h>

Braccio arm;

void setup() {
  Serial.begin(115200);   // faster serial = less time spent printing

  // Macros defined by the build system for this board:
#ifdef ARDUINO
  Serial.print(F("Arduino IDE/CLI version macro: "));
  Serial.println(ARDUINO);
#endif
#ifdef ARDUINO_AVR_UNO
  Serial.println(F("Board: Arduino UNO"));
#endif
  Serial.print(F("CPU clock (F_CPU): "));
#ifdef F_CPU
  Serial.println((unsigned long)F_CPU);
#else
  Serial.println(F("unknown"));
#endif

  arm.begin();
  arm.setAllAbsolute(0, 15, 0, 0, 0, 10);   // a long move so update() has work to do
}

void loop() {
  // Time 100 calls to update()
  unsigned long start = micros();
  for (int i = 0; i < 100; i++) {
    arm.update();
  }
  unsigned long elapsed = micros() - start;

  Serial.print(F("100 x update() took "));
  Serial.print(elapsed);
  Serial.print(F(" us -> "));
  Serial.print(elapsed / 100);
  Serial.println(F(" us per call"));

  arm.setAllAbsolute(180, 165, 180, 180, 180, 73);
  arm.safeDelay(3000);
  arm.setAllAbsolute(0, 15, 0, 0, 0, 10);
}
