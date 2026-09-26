// arm_moves.h - DECLARATIONS of our reusable arm actions (the "menu").
#ifndef ARM_MOVES_H
#define ARM_MOVES_H

#include <BraccioV2.h>

// The arm object is DEFINED in L05_multifile.ino.
// 'extern' says: "it exists somewhere else, trust me - the linker will find it".
extern Braccio arm;

const int GRIPPER_OPEN = 10;
const int GRIPPER_CLOSED = 73;

void goHome();
void openGripper();
void closeGripper();
void wave(int times);

#endif
