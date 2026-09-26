// Unit tests for MyBraccio that run on your PC - no arm, no Arduino, no risk.
//
// Build and run from the examples/pc_tests folder:
//   g++ -std=c++17 -Wall -Wextra -I fake -I ../libraries/MyBraccio/src \
//       test_mybraccio.cpp ../libraries/MyBraccio/src/MyBraccio.cpp -o test_mybraccio
//   ./test_mybraccio
#include <cstdio>
#include "MyBraccio.h"

// ---- the fake hardware's state ------------------------------------------
namespace fake {
unsigned long nowMs = 0;
int pinLevel[20] = {};
int servoAngle[20];
int servoWrites = 0;
}

// ---- a tiny test framework (about 20 lines) -----------------------------
static int checks = 0;
static int failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    checks++;                                                              \
    if (!(cond)) {                                                         \
      failures++;                                                          \
      std::printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);        \
    }                                                                      \
  } while (0)

#define CHECK_EQ(a, b)                                                     \
  do {                                                                     \
    checks++;                                                              \
    long _a = (a), _b = (b);                                               \
    if (_a != _b) {                                                        \
      failures++;                                                          \
      std::printf("  FAIL %s:%d: %s == %ld, expected %ld\n",               \
                  __FILE__, __LINE__, #a, _a, _b);                         \
    }                                                                      \
  } while (0)

static void resetFakes() {
  fake::nowMs = 0;
  fake::servoWrites = 0;
  for (int& a : fake::servoAngle) a = -1;
}

// Advance the fake clock, calling update() every millisecond like a busy loop() would.
static void runFor(MyBraccio& arm, unsigned long ms) {
  for (unsigned long i = 0; i < ms; i++) {
    fake::nowMs++;
    arm.update();
  }
}

// ---- the tests ----------------------------------------------------------
static void test_begin_holds_home_and_powers_on() {
  std::printf("begin() holds the home pose and switches power on\n");
  resetFakes();
  MyBraccio arm;
  arm.begin();
  CHECK(arm.started());
  CHECK_EQ(fake::servoAngle[11], 90);      // base pin
  CHECK_EQ(fake::servoAngle[3], 50);       // gripper pin
  CHECK_EQ(fake::pinLevel[12], HIGH);      // soft-start finished with power ON
  CHECK(!arm.isMoving());
}

static void test_setTarget_clamps_and_reports() {
  std::printf("setTarget clamps to the limits and reports correctly (BraccioV2 bug 1)\n");
  resetFakes();
  MyBraccio arm;
  CHECK(arm.setTarget(MyBraccio::ELBOW, 90));          // inside range -> true
  CHECK(!arm.setTarget(MyBraccio::BASE, -40));         // clamped -> false
  CHECK_EQ(arm.target(MyBraccio::BASE), 0);
  CHECK(!arm.setTarget(MyBraccio::GRIPPER, 200));
  CHECK_EQ(arm.target(MyBraccio::GRIPPER), 73);
}

static void test_invalid_joint_is_rejected() {
  std::printf("invalid joint indexes are rejected (bug 4)\n");
  MyBraccio arm;
  CHECK(!arm.setTarget(6, 90));
  CHECK(!arm.setTarget(200, 90));
  CHECK(!arm.nudge(7, 5));
  CHECK_EQ(arm.angle(9), -1);
}

static void test_speed_never_overshoots() {
  std::printf("speed > 1 arrives exactly, without overshoot (bug 2)\n");
  resetFakes();
  MyBraccio arm;
  arm.begin();
  CHECK(arm.setSpeed(MyBraccio::BASE, 7));
  arm.setTarget(MyBraccio::BASE, 100);
  runFor(arm, 10);
  CHECK_EQ(arm.angle(MyBraccio::BASE), 97);
  runFor(arm, 10);
  CHECK_EQ(arm.angle(MyBraccio::BASE), 100);           // not 104!
  runFor(arm, 100);
  CHECK_EQ(arm.angle(MyBraccio::BASE), 100);
  CHECK(!arm.isMoving());
}

static void test_bad_configuration_is_rejected() {
  std::printf("bad speed and limits are rejected (bug 6)\n");
  MyBraccio arm;
  CHECK(!arm.setSpeed(MyBraccio::BASE, 0));
  CHECK(!arm.setSpeed(MyBraccio::BASE, 50));
  CHECK(!arm.setLimits(MyBraccio::SHOULDER, 170, 20)); // min > max
  CHECK(!arm.setLimits(MyBraccio::SHOULDER, -5, 90));
  CHECK(arm.setLimits(MyBraccio::SHOULDER, 30, 120));
  CHECK_EQ(arm.target(MyBraccio::SHOULDER), 90);       // 90 still inside 30..120
  CHECK(arm.setLimits(MyBraccio::SHOULDER, 100, 120));
  CHECK_EQ(arm.target(MyBraccio::SHOULDER), 100);      // invariant kept: target re-clamped
}

static void test_update_is_self_timed() {
  std::printf("update() only steps every 10 ms, however often it is called\n");
  resetFakes();
  MyBraccio arm;
  arm.begin();
  arm.setTarget(MyBraccio::ELBOW, 120);
  int before = arm.angle(MyBraccio::ELBOW);
  for (int i = 0; i < 1000; i++) arm.update();         // 1000 calls, clock not moving
  CHECK_EQ(arm.angle(MyBraccio::ELBOW), before);
  runFor(arm, 100);                                    // 100 ms -> 10 steps
  CHECK_EQ(arm.angle(MyBraccio::ELBOW), before + 10);
}

static void test_millis_rollover() {
  std::printf("timing survives millis() rolling over (bug 3)\n");
  resetFakes();
  MyBraccio arm;
  arm.begin();
  fake::nowMs = 4294967295UL - 50;                     // 50 ms before rollover
  arm.update();                                        // re-sync the step timer
  arm.setTarget(MyBraccio::WRIST, 130);
  runFor(arm, 200);                                    // clock wraps to ~150 during this
  CHECK_EQ(arm.angle(MyBraccio::WRIST), 110);          // 20 steps of 1 degree
}

static void test_begin_with_pose_never_starts_at_zero() {
  std::printf("begin(pose) starts where told, never at 0 (bug 5)\n");
  resetFakes();
  MyBraccio arm;
  const int start[6] = {90, 45, 180, 180, 90, 10};
  arm.begin(start);
  CHECK_EQ(arm.angle(MyBraccio::SHOULDER), 45);
  arm.setTarget(MyBraccio::SHOULDER, 50);
  runFor(arm, 10);
  CHECK_EQ(fake::servoAngle[10], 46);                  // one step from 45, not a jump from 0
}

int main() {
  test_begin_holds_home_and_powers_on();
  test_setTarget_clamps_and_reports();
  test_invalid_joint_is_rejected();
  test_speed_never_overshoots();
  test_bad_configuration_is_rejected();
  test_update_is_self_timed();
  test_millis_rollover();
  test_begin_with_pose_never_starts_at_zero();

  std::printf("\n%d checks, %d failed\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
