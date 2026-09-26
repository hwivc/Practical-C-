// Lesson 01 - Hello, Arm!
// Prints build information (filled in by the preprocessor and compiler),
// then waves once and parks the arm.
#include <BraccioV2.h>

Braccio arm;

void setup() {
  Serial.begin(9600);

  // __DATE__, __TIME__ and __FILE__ are replaced with text by the
  // preprocessor at the moment you click Verify / Upload.
  Serial.print("Sketch: ");
  Serial.println(__FILE__);
  Serial.print("Compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);

  Serial.println("Powering up...");
  arm.begin();

  Serial.println("Hello! Waving now.");
  arm.setAllAbsolute(90, 90, 90, 60, 90, 50);   // wrist down
  arm.safeDelay(800);
  arm.setAllAbsolute(90, 90, 90, 120, 90, 50);  // wrist up
  arm.safeDelay(800);
  arm.setAllAbsolute(90, 90, 90, 60, 90, 50);
  arm.safeDelay(800);
  arm.setAllAbsolute(90, 90, 90, 90, 90, 50);   // back to centre
  arm.safeDelay(800);
  Serial.println("Done.");
}

void loop() {
  // Nothing to repeat. The arm holds its last pose.
}
