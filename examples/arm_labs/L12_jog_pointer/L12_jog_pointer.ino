// Lesson 12 - Pointers: a jog controller that "points at" the selected joint.
// Serial Monitor: 1-6 selects a joint, + / - moves it 5 degrees, ? shows all.
#include <BraccioV2.h>

Braccio arm;

struct JointState {
  const char* name;
  int index;      // BraccioV2 joint number
  int minAngle;
  int maxAngle;
  int angle;
};

JointState base     = {"base",      BASE_ROT,   0, 180, 90};
JointState shoulder = {"shoulder",  SHOULDER,  15, 165, 90};
JointState elbow    = {"elbow",     ELBOW,      0, 180, 90};
JointState wrist    = {"wrist",     WRIST,      0, 180, 90};
JointState wristRot = {"wrist rot", WRIST_ROT,  0, 180, 90};
JointState gripper  = {"gripper",   GRIPPER,   10,  73, 50};

JointState* selected = &base;   // a POINTER: holds the address of the chosen joint

// Takes a pointer, so it can change the caller's JointState.
void jog(JointState* j, int degrees) {
  if (j == nullptr) return;                 // always check before dereferencing
  int next = j->angle + degrees;            // -> reads a member through a pointer
  if (next < j->minAngle) next = j->minAngle;
  if (next > j->maxAngle) next = j->maxAngle;
  j->angle = next;
  arm.setOneAbsolute(j->index, j->angle);
}

void show(const JointState* j) {            // pointer to const: read-only access
  Serial.print(j == selected ? "> " : "  ");
  Serial.print(j->name);
  Serial.print(" = ");
  Serial.println(j->angle);
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  Serial.println("1-6 select joint, + / - jog 5 deg, ? show all");
}

void loop() {
  arm.update();
  delay(10);

  if (Serial.available() == 0) return;
  char c = Serial.read();

  switch (c) {
    case '1': selected = &base;     break;
    case '2': selected = &shoulder; break;
    case '3': selected = &elbow;    break;
    case '4': selected = &wrist;    break;
    case '5': selected = &wristRot; break;
    case '6': selected = &gripper;  break;
    case '+': jog(selected, +5);    break;
    case '-': jog(selected, -5);    break;
    case '?':
      show(&base); show(&shoulder); show(&elbow);
      show(&wrist); show(&wristRot); show(&gripper);
      return;
    default:
      return;                               // ignore newlines and other keys
  }
  show(selected);
}
