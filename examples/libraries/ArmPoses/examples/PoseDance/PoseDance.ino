// PoseDance - example for the ArmPoses library.
// File > Examples > ArmPoses > PoseDance
#include <BraccioV2.h>
#include <ArmPoses.h>

Braccio arm;
PosePlayer player(arm);

const Pose DANCE[] = {
  {60, 80, 100, 70, 45, 10},
  {120, 80, 100, 110, 135, 73},
  {90, 100, 70, 90, 90, 40},
  Poses::HOME,
};
const int DANCE_LENGTH = sizeof(DANCE) / sizeof(DANCE[0]);

void setup() {
  arm.begin();
}

void loop() {
  player.play(DANCE, DANCE_LENGTH, 1200);
  player.moveTo(Poses::PARK, 2000);
  arm.safeDelay(1000);
}
