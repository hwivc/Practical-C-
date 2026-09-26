// Lesson 08 - Your first class: a Gripper that knows its own state.
#include <BraccioV2.h>

Braccio arm;

class Gripper {
public:
  // Member variables (the object's data)
  int openAngle = 10;
  int closedAngle = 73;
  bool closed = false;
  int grabCount = 0;

  // Member functions / methods (what the object can do)
  void open() {
    arm.setOneAbsolute(GRIPPER, openAngle);
    arm.safeDelay(600);
    closed = false;
  }

  void close() {
    arm.setOneAbsolute(GRIPPER, closedAngle);
    arm.safeDelay(600);
    closed = true;
    grabCount++;
  }

  void toggle() {
    if (closed) {
      open();
    } else {
      close();
    }
  }

  void report();   // declared here, defined below the class
};

// Defining a method outside the class: ClassName::methodName
void Gripper::report() {
  Serial.print("Gripper is ");
  Serial.print(closed ? "CLOSED" : "OPEN");
  Serial.print(", grabs so far: ");
  Serial.println(grabCount);
}

Gripper hand;   // an OBJECT (instance) of the class Gripper

void setup() {
  Serial.begin(9600);
  arm.begin();
  hand.closedAngle = 60;   // hold something soft: don't squeeze all the way
}

void loop() {
  hand.toggle();
  hand.report();
  arm.safeDelay(1000);
}
