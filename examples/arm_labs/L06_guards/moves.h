// moves.h - arm actions that work with Pose.
#ifndef MOVES_H
#define MOVES_H

#include <BraccioV2.h>
#include "pose.h"   // moves.h needs Pose, so it includes pose.h itself

extern Braccio arm;

void moveToPose(Pose p, unsigned long waitMs);
void printPose(Pose p);

#endif  // MOVES_H
