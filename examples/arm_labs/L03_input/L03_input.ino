// Lesson 03 - A pose menu controlled from the Serial Monitor.
// Type 1, 2, 3 or 4 and press Enter.
#include <BraccioV2.h>

Braccio arm;

void setup() {
  Serial.begin(9600);
  arm.begin();
  Serial.println("Pose menu: 1=home  2=reach forward  3=look left  4=park");
}

void loop() {
  arm.update();   // keep the joints moving toward their targets
  delay(10);

  if (Serial.available() > 0) {
    char key = Serial.read();

    if (key == '\n' || key == '\r') {
      return;     // ignore the Enter key
    }

    switch (key) {
      case '1':
        Serial.println("Home");
        arm.setAllAbsolute(90, 90, 90, 90, 90, 50);
        break;
      case '2':
        Serial.println("Reach forward");
        arm.setAllAbsolute(90, 60, 60, 60, 90, 10);
        break;
      case '3':
        Serial.println("Look left");
        arm.setAllAbsolute(150, 90, 90, 90, 90, 50);
        break;
      case '4':
        Serial.println("Park");
        arm.setAllAbsolute(90, 45, 180, 180, 90, 10);
        break;
      default:
        Serial.print("Unknown key: ");
        Serial.println(key);
        break;
    }
  }
}
