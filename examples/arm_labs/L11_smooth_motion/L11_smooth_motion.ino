// Lesson 11 - Our own motion engine: SmoothJoint objects with const getters,
// overloaded methods, a static counter, and ease-out motion.
#include <BraccioV2.h>

Braccio arm;

class SmoothJoint {
public:
  SmoothJoint(int minA, int maxA, int home)
      : _min(minA), _max(maxA), _current(home), _target(home) {
    _count++;
  }

  // Overloads: absolute target, or relative with a flag
  void moveTo(int target) { _target = _clamp(target); }
  void moveTo(int amount, bool relative) {
    moveTo(relative ? _target + amount : amount);
  }

  // One step of motion. Ease-out: big steps when far, 1-degree steps when close.
  void update() {
    int remaining = _target - _current;
    if (remaining == 0) return;
    int step = remaining / 6;                  // move 1/6 of the remaining distance
    if (step == 0) step = (remaining > 0) ? 1 : -1;
    _current += step;                          // never overshoots: |step| <= |remaining|
  }

  // const methods: they only READ the object
  int angle() const { return _current; }
  bool isMoving() const { return _current != _target; }

  static int count() { return _count; }        // belongs to the class, not an object

private:
  int _clamp(int a) const { return a < _min ? _min : (a > _max ? _max : a); }

  static int _count;                           // shared by ALL SmoothJoint objects
  const int _min;
  const int _max;
  int _current;
  int _target;
};

int SmoothJoint::_count = 0;                   // static members are defined once, outside the class

SmoothJoint base(0, 180, 90);
SmoothJoint shoulder(15, 165, 90);
SmoothJoint elbow(0, 180, 90);
SmoothJoint wrist(0, 180, 90);
SmoothJoint wristRot(0, 180, 90);
SmoothJoint gripper(10, 73, 50);

void updateAll() {
  base.update(); shoulder.update(); elbow.update();
  wrist.update(); wristRot.update(); gripper.update();
  // Our objects already clamp, so writing directly with setAllNow is safe.
  arm.setAllNow(base.angle(), shoulder.angle(), elbow.angle(),
                wrist.angle(), wristRot.angle(), gripper.angle());
}

bool anyMoving() {
  return base.isMoving() || shoulder.isMoving() || elbow.isMoving() ||
         wrist.isMoving() || wristRot.isMoving() || gripper.isMoving();
}

void waitUntilArrived() {
  while (anyMoving()) {
    updateAll();
    delay(20);
  }
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  Serial.print("Joints created: ");
  Serial.println(SmoothJoint::count());
}

void loop() {
  base.moveTo(30);
  elbow.moveTo(60);
  gripper.moveTo(73);
  waitUntilArrived();
  delay(500);

  base.moveTo(120, true);       // relative: +120 from 30 -> 150
  elbow.moveTo(120);
  gripper.moveTo(10);
  waitUntilArrived();
  delay(500);
}
