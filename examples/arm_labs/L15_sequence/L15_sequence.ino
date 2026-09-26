// Lesson 15 - Arrays: joint limits in 1-D arrays, a whole routine in a 2-D array.
#include <BraccioV2.h>

Braccio arm;

const int NUM_JOINTS = 6;
const char* const JOINT_NAMES[NUM_JOINTS] = {"base", "shoulder", "elbow", "wrist", "wristRot", "gripper"};
const int MIN_ANGLE[NUM_JOINTS] = {0, 15, 0, 0, 0, 10};
const int MAX_ANGLE[NUM_JOINTS] = {180, 165, 180, 180, 180, 73};

// A routine: each row is one pose (base, shoulder, elbow, wrist, wristRot, gripper) + hold time.
const int ROUTINE[][NUM_JOINTS + 1] = {
  { 90,  90,  90,  90,  90, 50,  800},   // home
  { 60,  70, 110,  60,  90, 10, 1200},   // reach left, gripper open
  { 60,  70, 110,  60,  90, 73,  600},   // grab
  {120,  95,  80, 100,  30, 73, 1500},   // carry right, twist wrist
  {120,  80,  95,  80, 150, 73,  800},   // twist the other way
  {120,  80,  95,  80, 150, 10,  600},   // release
  { 90,  90,  90,  90,  90, 50, 1000},   // home
};
// sizeof(whole array) / sizeof(one row) = number of rows, computed by the compiler
const int ROUTINE_LENGTH = sizeof(ROUTINE) / sizeof(ROUTINE[0]);

// Arrays are passed as a pointer to their first element: the function can't know
// the length, so we pass it too. 'const' = read-only.
bool poseIsSafe(const int pose[], int count) {
  for (int j = 0; j < count; j++) {
    if (pose[j] < MIN_ANGLE[j] || pose[j] > MAX_ANGLE[j]) {
      Serial.print("  unsafe ");
      Serial.print(JOINT_NAMES[j]);
      Serial.print(" = ");
      Serial.println(pose[j]);
      return false;
    }
  }
  return true;
}

void playRoutine() {
  for (int step = 0; step < ROUTINE_LENGTH; step++) {
    const int* row = ROUTINE[step];          // pointer to the start of this row
    Serial.print("Step ");
    Serial.print(step + 1);
    Serial.print("/");
    Serial.println(ROUTINE_LENGTH);

    if (!poseIsSafe(row, NUM_JOINTS)) {
      Serial.println("  skipped");
      continue;
    }
    arm.setAllAbsolute(row[0], row[1], row[2], row[3], row[4], row[5]);
    arm.safeDelay(row[NUM_JOINTS]);          // the 7th column is the hold time
  }
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  Serial.print("Routine has ");
  Serial.print(ROUTINE_LENGTH);
  Serial.println(" steps");
}

void loop() {
  playRoutine();
  arm.safeDelay(2000);
}
