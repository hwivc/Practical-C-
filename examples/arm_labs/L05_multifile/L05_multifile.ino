// Lesson 05 - A sketch split into three files (three tabs in the Arduino IDE).
#include "arm_moves.h"

Braccio arm;   // the one and only DEFINITION of the arm object

void setup() {
  Serial.begin(9600);
  arm.begin();
  wave(2);
}

void loop() {
  openGripper();
  closeGripper();
  goHome();
}
