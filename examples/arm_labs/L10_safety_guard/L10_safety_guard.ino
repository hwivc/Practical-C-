// Lesson 10 - Encapsulation: every move must pass through a SafetyGuard.
// The guard keeps its rules and counters private, so nobody can bypass them.
#include <BraccioV2.h>

Braccio arm;

class SafetyGuard {
public:
  explicit SafetyGuard(Braccio& a) : _arm(a) {}

  // The ONLY way to move the arm. Returns true if the pose was accepted.
  bool requestPose(int base, int shoulder, int elbow, int wrist, int wristRot, int gripper) {
    if (_locked) {
      _rejected++;
      Serial.println("  REJECTED: guard is locked");
      return false;
    }
    if (!_isOutsideTableZone(shoulder, elbow)) {
      _rejected++;
      Serial.println("  REJECTED: pose would hit the table");
      return false;
    }
    _arm.setAllAbsolute(base, shoulder, elbow, wrist, wristRot, gripper);
    _accepted++;
    return true;
  }

  void lock()   { _locked = true; }
  void unlock() { _locked = false; }

  // Read-only access to the statistics (getters): no setters on purpose!
  int accepted() const { return _accepted; }
  int rejected() const { return _rejected; }

private:
  // Rule: leaning the shoulder far forward AND bending the elbow down
  // drives the gripper into the table. (Tune the numbers for YOUR arm.)
  bool _isOutsideTableZone(int shoulder, int elbow) const {
    bool shoulderLow = shoulder < 50;
    bool elbowLow = elbow < 60;
    return !(shoulderLow && elbowLow);
  }

  Braccio& _arm;
  bool _locked = false;
  int _accepted = 0;
  int _rejected = 0;
};

SafetyGuard guard(arm);

void tryPose(const char* label, int b, int s, int e, int w, int wr, int g) {
  Serial.print("Pose ");
  Serial.println(label);
  if (guard.requestPose(b, s, e, w, wr, g)) {
    arm.safeDelay(1500);
  }
}

void setup() {
  Serial.begin(9600);
  arm.begin();
}

void loop() {
  tryPose("reach", 90, 70, 70, 70, 90, 10);    // fine
  tryPose("dig", 90, 40, 40, 90, 90, 10);      // shoulder AND elbow low: rejected

  guard.lock();
  tryPose("while locked", 60, 90, 90, 90, 90, 50);   // rejected
  guard.unlock();

  tryPose("home", 90, 90, 90, 90, 90, 50);     // fine

  Serial.print("accepted: ");
  Serial.print(guard.accepted());
  Serial.print("  rejected: ");
  Serial.println(guard.rejected());
  // guard._rejected = 0;   // won't compile: _rejected is private
}
