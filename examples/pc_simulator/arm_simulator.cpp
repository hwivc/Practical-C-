// Project P1 - A console simulator of the Braccio arm.
// Build:  g++ -std=c++17 -Wall -Wextra arm_simulator.cpp -o arm_simulator
// Run:    ./arm_simulator
#include <cmath>
#include <cstdio>

// ------------------------------------------------------------------ Joint
class Joint {
public:
  Joint() : Joint("joint", 0, 180, 90) {}
  Joint(const char* name, int minAngle, int maxAngle, int home)
      : _name(name), _min(minAngle), _max(maxAngle), _current(home), _target(home) {}

  bool setTarget(int angle) {
    _target = angle < _min ? _min : (angle > _max ? _max : angle);
    return _target == angle;
  }

  bool setSpeed(int degreesPerStep) {
    if (degreesPerStep < 1 || degreesPerStep > 10) return false;
    _speed = degreesPerStep;
    return true;
  }

  void update() {
    int remaining = _target - _current;
    if (remaining > _speed) remaining = _speed;
    if (remaining < -_speed) remaining = -_speed;
    _current += remaining;
  }

  const char* name() const { return _name; }
  int angle() const { return _current; }
  int minAngle() const { return _min; }
  int maxAngle() const { return _max; }
  bool isMoving() const { return _current != _target; }

private:
  const char* _name;
  int _min, _max;
  int _current, _target;
  int _speed = 1;
};

// ------------------------------------------------------------------ Arm
class ArmSim {
public:
  static const int NUM_JOINTS = 6;
  enum { BASE, SHOULDER, ELBOW, WRIST, WRIST_ROT, GRIPPER };

  // Approximate Braccio link lengths in millimetres. Measure your own arm!
  static constexpr double BASE_HEIGHT = 71.5;   // table to shoulder axis
  static constexpr double UPPER_ARM = 125.0;    // shoulder to elbow
  static constexpr double FOREARM = 125.0;      // elbow to wrist
  static constexpr double HAND = 195.0;         // wrist to gripper tip

  ArmSim()
      : _joints{Joint("base", 0, 180, 90),    Joint("shoulder", 15, 165, 90),
                Joint("elbow", 0, 180, 90),   Joint("wrist", 0, 180, 90),
                Joint("wristRot", 0, 180, 90), Joint("gripper", 10, 73, 50)} {}

  bool setPose(const int pose[NUM_JOINTS]) {
    bool exact = true;
    for (int j = 0; j < NUM_JOINTS; j++) exact = _joints[j].setTarget(pose[j]) && exact;
    return exact;
  }

  Joint& joint(int j) { return _joints[j]; }

  void update() {
    for (Joint& j : _joints) j.update();
    _timeMs += 10;   // one simulated step = 10 ms, like BraccioV2's safeDelay
  }

  bool isMoving() const {
    for (const Joint& j : _joints)
      if (j.isMoving()) return true;
    return false;
  }

  // Forward kinematics: joint angles -> gripper tip position.
  // Model: servo 90 = link straight up; shoulder angle measured from horizontal-forward;
  // elbow and wrist bend relative to the previous link.
  void gripperPosition(double& x, double& y, double& z) const {
    const double D2R = 3.14159265358979323846 / 180.0;   // M_PI is not standard C++
    double a1 = _joints[SHOULDER].angle() * D2R;
    double a2 = a1 + (_joints[ELBOW].angle() - 90) * D2R;
    double a3 = a2 + (_joints[WRIST].angle() - 90) * D2R;
    double reach = UPPER_ARM * std::cos(a1) + FOREARM * std::cos(a2) + HAND * std::cos(a3);
    z = BASE_HEIGHT + UPPER_ARM * std::sin(a1) + FOREARM * std::sin(a2) + HAND * std::sin(a3);
    double base = _joints[BASE].angle() * D2R;   // 0 = right (+x), 90 = forward (+y)
    x = reach * std::cos(base);
    y = reach * std::sin(base);
  }

  void print() const {
    std::printf("t = %5lu ms\n", _timeMs);
    for (const Joint& j : _joints) {
      // A 20-character gauge: where the joint sits inside its own limits.
      int filled = (j.angle() - j.minAngle()) * 20 / (j.maxAngle() - j.minAngle());
      char bar[21];
      for (int i = 0; i < 20; i++) bar[i] = i < filled ? '#' : '.';
      bar[20] = '\0';
      std::printf("  %-9s [%s] %3d%s\n", j.name(), bar, j.angle(), j.isMoving() ? "  moving" : "");
    }
    double x, y, z;
    gripperPosition(x, y, z);
    std::printf("  gripper tip at x=%6.1f  y=%6.1f  z=%6.1f mm\n\n", x, y, z);
  }

private:
  Joint _joints[NUM_JOINTS];
  unsigned long _timeMs = 0;
};

// ------------------------------------------------------------------ program
int main() {
  ArmSim arm;
  const int ROUTINE[][ArmSim::NUM_JOINTS] = {
      {90, 90, 90, 90, 90, 50},     // upright
      {45, 60, 120, 60, 90, 10},    // reach forward-right, open
      {45, 60, 120, 60, 90, 73},    // grab
      {135, 100, 80, 100, 90, 73},  // carry left
      {90, 45, 180, 180, 90, 10},   // park (the official safety pose)
  };
  const int STEPS = sizeof(ROUTINE) / sizeof(ROUTINE[0]);

  std::puts("=== Braccio simulator ===");
  arm.print();
  arm.joint(ArmSim::BASE).setSpeed(2);

  for (int s = 1; s < STEPS; s++) {
    std::printf("--- step %d: new pose ---\n", s);
    if (!arm.setPose(ROUTINE[s])) std::puts("  (some angles were clamped)");
    int ticks = 0;
    while (arm.isMoving()) {
      arm.update();
      if (++ticks % 30 == 0) arm.print();   // print every 300 simulated ms
    }
    arm.print();                            // final position of this step
  }
  return 0;
}
