// Lesson 02 - Variables, types and constants on a real microcontroller
#include <BraccioV2.h>

Braccio arm;

// Named constants: easy to read, impossible to change by accident.
const int BASE_CENTER = 90;
const int SWING = 40;               // how far to swing each side (degrees)
const unsigned long HOLD_MS = 1500; // time values should be unsigned long

int swings = 0;                     // global: remembered between loop() calls

void setup() {
  Serial.begin(9600);

  // How big is each type on THIS chip?
  Serial.print("sizeof(int)    = "); Serial.println((int)sizeof(int));
  Serial.print("sizeof(long)   = "); Serial.println((int)sizeof(long));
  Serial.print("sizeof(float)  = "); Serial.println((int)sizeof(float));
  Serial.print("sizeof(double) = "); Serial.println((int)sizeof(double));

  // Overflow demo: int16_t always has 16 bits, on every computer.
  int16_t big = 32767;
  big = big + 1;
  Serial.print("32767 + 1 in an int16_t = ");
  Serial.println(big);

  arm.begin();
}

void loop() {
  int left = BASE_CENTER + SWING;   // local: created fresh on every loop()
  int right = BASE_CENTER - SWING;

  arm.setOneAbsolute(BASE_ROT, left);
  arm.safeDelay(HOLD_MS);
  arm.setOneAbsolute(BASE_ROT, right);
  arm.safeDelay(HOLD_MS);

  swings = swings + 1;
  Serial.print("Swings so far: ");
  Serial.println(swings);
}
