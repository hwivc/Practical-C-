// Lesson 20 - Debugging with the Serial Plotter: watch every joint move as a graph.
// Tools > Serial Plotter, 115200 baud.
#include <MyBraccio.h>

MyBraccio arm;

const int POSES[][MyBraccio::NUM_JOINTS] = {
  {40, 70, 120, 60, 90, 10},
  {140, 110, 60, 120, 30, 73},
  {90, 90, 90, 90, 90, 50},
};
const int NUM_POSES = sizeof(POSES) / sizeof(POSES[0]);
const char* const LABELS[MyBraccio::NUM_JOINTS] = {"base", "shoulder", "elbow", "wrist", "wristRot", "gripper"};

int next = 0;
unsigned long lastPrint = 0;

// One line of "label:value,label:value" = one point per joint on the Serial Plotter.
void plotAngles() {
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
    Serial.print(LABELS[j]);
    Serial.print(':');
    Serial.print(arm.angle(j));
    Serial.print(j < MyBraccio::NUM_JOINTS - 1 ? "," : "\n");
  }
}

void setup() {
  Serial.begin(115200);
  arm.begin();
  arm.setSpeed(MyBraccio::BASE, 2);
}

void loop() {
  arm.update();

  if (!arm.isMoving()) {
    arm.setAll(POSES[next]);
    next = (next + 1) % NUM_POSES;
  }

  if (millis() - lastPrint >= 50) {   // 20 points per second is plenty
    lastPrint = millis();
    plotAngles();
  }
}
