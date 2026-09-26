// pose.h - a type that groups the six joint angles of one arm pose.
#ifndef POSE_H
#define POSE_H

struct Pose {
  int base;
  int shoulder;
  int elbow;
  int wrist;
  int wristRot;
  int gripper;
};

// Two poses everyone needs. 'const' globals are allowed in headers.
const Pose HOME = {90, 90, 90, 90, 90, 50};
const Pose PARK = {90, 45, 180, 180, 90, 10};

#endif  // POSE_H
