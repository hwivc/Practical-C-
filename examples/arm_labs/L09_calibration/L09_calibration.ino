// Lesson 09 - Constructors: describe each joint's calibration as an object,
// then apply it to the arm.
#include <BraccioV2.h>

class JointConfig {
public:
  // Constructor with a member initializer list
  JointConfig(const char* name, int index, int minAngle, int maxAngle, int center)
      : _name(name), _index(index), _min(minAngle), _max(maxAngle), _center(center) {
    // The body runs AFTER the members are initialised.
    // Don't touch hardware here: global objects are built before setup().
  }

  // Send this joint's limits and centre to the arm.
  void applyTo(Braccio& arm) const {
    arm.setJointMin(_index, _min);
    arm.setJointMax(_index, _max);
    arm.setJointCenter(_index, _center);
  }

  void print() const {
    Serial.print(_name);
    Serial.print(": min ");
    Serial.print(_min);
    Serial.print(", max ");
    Serial.print(_max);
    Serial.print(", centre ");
    Serial.println(_center);
  }

  int center() const { return _center; }

private:
  const char* _name;
  const int _index;   // const members MUST be set in the initializer list
  int _min;
  int _max;
  int _center;
};

// ---- Your arm's calibration: edit these numbers for YOUR arm ------------
//                     name         index      min  max  centre
JointConfig baseCfg("base",      BASE_ROT,    0, 180,  90);
JointConfig shoulderCfg("shoulder", SHOULDER, 15, 165,  90);
JointConfig elbowCfg("elbow",    ELBOW,       0, 180,  90);
JointConfig wristCfg("wrist",    WRIST,       0, 180,  90);
JointConfig wristRotCfg("wristRot", WRIST_ROT, 0, 180,  90);
JointConfig gripperCfg("gripper", GRIPPER,   10,  73,  40);

Braccio arm;

void setup() {
  Serial.begin(9600);

  // Calibration must be applied BEFORE begin(), so the start pose uses the new centres.
  baseCfg.applyTo(arm);
  shoulderCfg.applyTo(arm);
  elbowCfg.applyTo(arm);
  wristCfg.applyTo(arm);
  wristRotCfg.applyTo(arm);
  gripperCfg.applyTo(arm);

  baseCfg.print();
  shoulderCfg.print();
  elbowCfg.print();
  wristCfg.print();
  wristRotCfg.print();
  gripperCfg.print();

  arm.begin();   // stands up using the calibrated centres
}

void loop() {
  // Move 30 degrees either side of the CALIBRATED centre of the base
  arm.setOneAbsolute(BASE_ROT, baseCfg.center() - 30);
  arm.safeDelay(1500);
  arm.setOneAbsolute(BASE_ROT, baseCfg.center() + 30);
  arm.safeDelay(1500);
}
