// Project P3 - Drive the arm by typing commands in the Serial Monitor.
// Line ending: Newline. Type HELP to see the commands.
// Needs the MyBraccio library (Lesson 19).
#include <MyBraccio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

MyBraccio arm;

// ---- Line input without String or the heap (Lesson 14) ------------------
const uint8_t MAX_LINE = 48;
char line[MAX_LINE];
uint8_t lineLength = 0;
bool lineTooLong = false;

// Returns true when a complete line is ready in `line`.
bool readLine() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r') continue;                 // ignore carriage returns
    if (c == '\n') {
      line[lineLength] = '\0';               // terminate the C string
      bool ok = !lineTooLong;
      lineLength = 0;
      lineTooLong = false;
      if (!ok) Serial.println(F("ERR line too long"));
      return ok;
    }
    if (lineLength < MAX_LINE - 1) {
      line[lineLength++] = (char)toupper(c); // commands are case-insensitive
    } else {
      lineTooLong = true;                    // keep reading until '\n', then reject
    }
  }
  return false;
}

// Parses a whole token as an int. Rejects "12abc" and empty tokens.
bool parseInt(const char* token, int& out) {
  if (token == nullptr || *token == '\0') return false;
  char* end;
  long v = strtol(token, &end, 10);
  if (*end != '\0' || v < -1000 || v > 1000) return false;
  out = (int)v;
  return true;
}

// ---- Command handlers ---------------------------------------------------
const char JOINT_LETTERS[] = "BSEWRG";   // index = MyBraccio joint number

void printPose() {
  const char* names[] = {"base", "shoulder", "elbow", "wrist", "wristRot", "gripper"};
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
    Serial.print(names[j]);
    Serial.print('=');
    Serial.print(arm.angle(j));
    if (arm.isMoving(j)) {
      Serial.print(F("->"));
      Serial.print(arm.target(j));
    }
    Serial.print(j < MyBraccio::NUM_JOINTS - 1 ? ' ' : '\n');
  }
}

void printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  B|S|E|W|R|G <angle>   move one joint (base shoulder elbow wrist rot gripper)"));
  Serial.println(F("  G OPEN | G CLOSE      open / close the gripper"));
  Serial.println(F("  MOVE b s e w r g      move all six joints"));
  Serial.println(F("  HOME | PARK | STOP    named poses / freeze"));
  Serial.println(F("  SPEED <1-10>          degrees per step for all joints"));
  Serial.println(F("  POSE | HELP"));
}

void reportClamp(bool exact) {
  Serial.println(exact ? F("OK") : F("OK (clamped to limits)"));
}

void handleLine() {
  char* cmd = strtok(line, " \t");           // first word
  if (cmd == nullptr) return;                // empty line

  // Single-joint command: one letter from JOINT_LETTERS
  const char* found = (strlen(cmd) == 1) ? strchr(JOINT_LETTERS, cmd[0]) : nullptr;
  if (found != nullptr) {
    uint8_t joint = found - JOINT_LETTERS;   // pointer arithmetic gives the index (Lesson 15)
    char* arg = strtok(nullptr, " \t");
    int angle;
    if (joint == MyBraccio::GRIPPER && arg != nullptr && strcmp(arg, "OPEN") == 0) {
      angle = arm.minAngle(joint);
    } else if (joint == MyBraccio::GRIPPER && arg != nullptr && strcmp(arg, "CLOSE") == 0) {
      angle = arm.maxAngle(joint);
    } else if (!parseInt(arg, angle)) {
      Serial.println(F("ERR expected an angle, e.g. B 120"));
      return;
    }
    reportClamp(arm.setTarget(joint, angle));
    return;
  }

  if (strcmp(cmd, "MOVE") == 0) {
    int pose[MyBraccio::NUM_JOINTS];
    for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
      if (!parseInt(strtok(nullptr, " \t"), pose[j])) {
        Serial.println(F("ERR MOVE needs 6 angles"));
        return;
      }
    }
    reportClamp(arm.setAll(pose));
  } else if (strcmp(cmd, "HOME") == 0) {
    arm.setAll(90, 90, 90, 90, 90, 50);
    Serial.println(F("OK"));
  } else if (strcmp(cmd, "PARK") == 0) {
    arm.setAll(90, 45, 180, 180, 90, 10);
    Serial.println(F("OK"));
  } else if (strcmp(cmd, "STOP") == 0) {
    arm.stop();
    Serial.println(F("OK stopped"));
  } else if (strcmp(cmd, "SPEED") == 0) {
    int s;
    if (!parseInt(strtok(nullptr, " \t"), s) || s < 1 || s > 10) {
      Serial.println(F("ERR SPEED needs 1-10"));
      return;
    }
    arm.setSpeedAll((uint8_t)s);
    Serial.println(F("OK"));
  } else if (strcmp(cmd, "POSE") == 0) {
    printPose();
  } else if (strcmp(cmd, "HELP") == 0) {
    printHelp();
  } else {
    Serial.print(F("ERR unknown command: "));
    Serial.println(cmd);
  }
}

// ---- Arduino entry points -----------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting arm..."));
  arm.begin();
  Serial.println(F("Ready. Type HELP."));
}

void loop() {
  arm.update();          // the arm keeps moving while we wait for input
  if (readLine()) {
    Serial.print(F("> "));
    Serial.println(line);
    handleLine();
  }
}
