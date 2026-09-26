// Project P4 - Teach & replay: jog the arm into poses, record them, play them back,
// and keep them in EEPROM so they survive power-off.
// Serial Monitor, line ending: Newline. Type HELP.
// Needs the MyBraccio library (Lesson 19).
#include <MyBraccio.h>
#include <EEPROM.h>
#include <string.h>
#include <ctype.h>

MyBraccio arm;

// ---- Recording storage (static allocation, Lesson 14) --------------------
struct Pose {
  int16_t angle[MyBraccio::NUM_JOINTS];
};

const uint8_t MAX_POSES = 30;
Pose poses[MAX_POSES];
uint8_t poseCount = 0;

// ---- EEPROM layout: [Header][Pose 0][Pose 1]... --------------------------
struct Header {
  uint16_t magic;     // tells us the EEPROM really contains OUR data
  uint8_t version;
  uint8_t count;
};
const uint16_t MAGIC = 0xB2AC;
const uint8_t VERSION = 1;

// ---- Playback state (non-blocking, like the P2 state machine) ------------
bool playing = false;
uint8_t playIndex = 0;
unsigned long arrivedAt = 0;
const unsigned long HOLD_MS = 400;

// ---- Jogging --------------------------------------------------------------
const char JOINT_KEYS[] = "bserwg";   // b=base s=shoulder e=elbow r=wrist-rot w=wrist g=gripper
const uint8_t KEY_TO_JOINT[] = {MyBraccio::BASE, MyBraccio::SHOULDER, MyBraccio::ELBOW,
                                MyBraccio::WRIST_ROT, MyBraccio::WRIST, MyBraccio::GRIPPER};
uint8_t selected = MyBraccio::BASE;
const int JOG_STEP = 5;

// ---- Line input (same technique as Project P3) ---------------------------
const uint8_t MAX_LINE = 40;
char line[MAX_LINE];
uint8_t lineLength = 0;

bool readLine() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      line[lineLength] = '\0';
      lineLength = 0;
      return true;
    }
    if (lineLength < MAX_LINE - 1) line[lineLength++] = c;
  }
  return false;
}

// ---- Helpers --------------------------------------------------------------
void printPose(const Pose& p) {
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
    Serial.print(p.angle[j]);
    Serial.print(j < MyBraccio::NUM_JOINTS - 1 ? ' ' : '\n');
  }
}

void goToPose(const Pose& p) {
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) arm.setTarget(j, p.angle[j]);
}

void saveCurrentPose() {
  if (poseCount >= MAX_POSES) {
    Serial.println(F("ERR memory full"));
    return;
  }
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
    poses[poseCount].angle[j] = arm.target(j);   // the target is where we've jogged to
  }
  Serial.print(F("Saved pose "));
  Serial.print(poseCount);
  Serial.print(F(": "));
  printPose(poses[poseCount]);
  poseCount++;
}

void storeToEeprom() {
  Header h = {MAGIC, VERSION, poseCount};
  EEPROM.put(0, h);
  for (uint8_t i = 0; i < poseCount; i++) {
    EEPROM.put(sizeof(Header) + i * sizeof(Pose), poses[i]);   // put() only writes changed bytes
  }
  Serial.print(F("Stored "));
  Serial.print(poseCount);
  Serial.println(F(" poses in EEPROM"));
}

bool loadFromEeprom() {
  Header h;
  EEPROM.get(0, h);
  if (h.magic != MAGIC || h.version != VERSION || h.count > MAX_POSES) {
    return false;                                   // empty or foreign data: don't trust it
  }
  for (uint8_t i = 0; i < h.count; i++) {
    EEPROM.get(sizeof(Header) + i * sizeof(Pose), poses[i]);
  }
  poseCount = h.count;
  return true;
}

void printHelp() {
  Serial.println(F("Jog:  a line of keys, e.g. 'b+++' or 'e--g+'"));
  Serial.println(F("      b=base s=shoulder e=elbow w=wrist r=wrist-rot g=gripper  +/- = 5 deg"));
  Serial.println(F("Cmd:  SAVE  LIST  PLAY  STOP  UNDO  CLEAR  STORE  LOAD  HELP"));
}

// ---- Command handling -----------------------------------------------------
void handleWord(const char* w) {
  if (strcmp(w, "SAVE") == 0) {
    saveCurrentPose();
  } else if (strcmp(w, "LIST") == 0) {
    for (uint8_t i = 0; i < poseCount; i++) {
      Serial.print(i);
      Serial.print(F(": "));
      printPose(poses[i]);
    }
    if (poseCount == 0) Serial.println(F("(no poses)"));
  } else if (strcmp(w, "PLAY") == 0) {
    if (poseCount == 0) {
      Serial.println(F("ERR nothing recorded"));
      return;
    }
    playing = true;
    playIndex = 0;
    arrivedAt = 0;
    goToPose(poses[0]);
    Serial.println(F("Playing..."));
  } else if (strcmp(w, "STOP") == 0) {
    playing = false;
    arm.stop();
    Serial.println(F("Stopped"));
  } else if (strcmp(w, "UNDO") == 0) {
    if (poseCount > 0) poseCount--;
    Serial.print(poseCount);
    Serial.println(F(" poses left"));
  } else if (strcmp(w, "CLEAR") == 0) {
    playing = false;
    poseCount = 0;
    Serial.println(F("Cleared (EEPROM unchanged until STORE)"));
  } else if (strcmp(w, "STORE") == 0) {
    storeToEeprom();
  } else if (strcmp(w, "LOAD") == 0) {
    Serial.println(loadFromEeprom() ? F("Loaded from EEPROM") : F("ERR no valid data in EEPROM"));
  } else if (strcmp(w, "HELP") == 0) {
    printHelp();
  } else {
    Serial.print(F("ERR unknown: "));
    Serial.println(w);
  }
}

void handleJogKeys(const char* keys) {
  for (const char* k = keys; *k != '\0'; k++) {   // walk the C string with a pointer
    const char* found = strchr(JOINT_KEYS, *k);
    if (found != nullptr) {
      selected = KEY_TO_JOINT[found - JOINT_KEYS];
    } else if (*k == '+') {
      arm.nudge(selected, JOG_STEP);
    } else if (*k == '-') {
      arm.nudge(selected, -JOG_STEP);
    }
  }
  Serial.print(F("target: "));
  for (uint8_t j = 0; j < MyBraccio::NUM_JOINTS; j++) {
    Serial.print(arm.target(j));
    Serial.print(j < MyBraccio::NUM_JOINTS - 1 ? ' ' : '\n');
  }
}

void handleLine() {
  if (line[0] == '\0') return;
  if (isupper(line[0])) {
    handleWord(line);       // commands are UPPERCASE words
  } else {
    handleJogKeys(line);    // everything else is a string of jog keys
  }
}

// ---- Playback, advanced from loop() without blocking ----------------------
void updatePlayback() {
  if (!playing || arm.isMoving()) return;
  if (arrivedAt == 0) {
    arrivedAt = millis();                   // just arrived: start the hold timer
    return;
  }
  if (millis() - arrivedAt < HOLD_MS) return;
  arrivedAt = 0;
  playIndex++;
  if (playIndex >= poseCount) {
    playing = false;
    Serial.println(F("Playback finished"));
    return;
  }
  goToPose(poses[playIndex]);
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  arm.setSpeedAll(2);
  if (loadFromEeprom()) {
    Serial.print(F("Loaded "));
    Serial.print(poseCount);
    Serial.println(F(" poses from EEPROM"));
  }
  printHelp();
}

void loop() {
  arm.update();
  updatePlayback();
  if (readLine()) {
    Serial.print(F("> "));
    Serial.println(line);
    handleLine();
  }
}
