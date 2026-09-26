// MyBraccio - NonBlocking example: the arm moves while the sketch keeps doing other work.
// The built-in LED keeps blinking and Serial stays responsive the whole time.
#include <MyBraccio.h>

MyBraccio arm;

const int POSES[][MyBraccio::NUM_JOINTS] = {
  {60, 80, 100, 70, 90, 10},
  {60, 80, 100, 70, 90, 73},
  {120, 95, 80, 100, 45, 73},
  {120, 95, 80, 100, 45, 10},
};
const int NUM_POSES = sizeof(POSES) / sizeof(POSES[0]);

int poseIndex = 0;
unsigned long lastBlink = 0;
bool ledOn = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);   // pin 13 is free on the Braccio shield
  arm.begin();
  arm.setAll(POSES[0]);
}

void loop() {
  arm.update();                   // returns immediately unless a step is due

  // Task 1: when the arm arrives, start the next pose
  if (!arm.isMoving()) {
    poseIndex = (poseIndex + 1) % NUM_POSES;
    arm.setAll(POSES[poseIndex]);
    Serial.print("Heading to pose ");
    Serial.println(poseIndex);
  }

  // Task 2: blink the LED every 250 ms, at the same time
  if (millis() - lastBlink >= 250) {
    lastBlink = millis();
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
  }

  // Task 3: 's' on the Serial Monitor stops the arm immediately
  if (Serial.available() && Serial.read() == 's') {
    arm.stop();
    Serial.println("Stopped!");
  }
}
