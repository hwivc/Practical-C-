// Project P2 - Pick & place as a finite-state machine.
// Moves three blocks from a pick-up spot and stacks them at a drop-off spot.
// Needs the MyBraccio library (Lesson 19).
#include <MyBraccio.h>

MyBraccio arm;

// ---- Teach these for YOUR table (use the jog controller from Lesson 12) ----
struct Pose {
  int base, shoulder, elbow, wrist, wristRot, gripper;
};

const int GRIP_OPEN = 10;
const int GRIP_CLOSED = 60;   // for a ~3 cm foam block; don't squeeze to 73

const Pose SAFE        = {90, 90, 90, 90, 90, GRIP_OPEN};   // high above everything
const Pose ABOVE_PICK  = {45, 80, 110, 60, 90, GRIP_OPEN};
const Pose AT_PICK     = {45, 60, 120, 55, 90, GRIP_OPEN};
const Pose ABOVE_PLACE = {135, 80, 110, 60, 90, GRIP_CLOSED};
// Drop-off height changes with the stack: one pose per level.
const Pose AT_PLACE[] = {
  {135, 60, 120, 55, 90, GRIP_CLOSED},   // level 0 (table)
  {135, 64, 118, 58, 90, GRIP_CLOSED},   // level 1
  {135, 68, 116, 61, 90, GRIP_CLOSED},   // level 2
};
const int NUM_BLOCKS = sizeof(AT_PLACE) / sizeof(AT_PLACE[0]);

// ---- The state machine --------------------------------------------------
enum class State : uint8_t {
  GoSafe, ApproachPick, DescendPick, Grab, LiftPick,
  Carry, DescendPlace, Release, LiftPlace, Done
};

State state = State::GoSafe;
int block = 0;                    // which block we're moving (0..NUM_BLOCKS-1)
unsigned long stateStart = 0;     // when we entered the current state

const char* stateName(State s) {
  switch (s) {
    case State::GoSafe:       return "GoSafe";
    case State::ApproachPick: return "ApproachPick";
    case State::DescendPick:  return "DescendPick";
    case State::Grab:         return "Grab";
    case State::LiftPick:     return "LiftPick";
    case State::Carry:        return "Carry";
    case State::DescendPlace: return "DescendPlace";
    case State::Release:      return "Release";
    case State::LiftPlace:    return "LiftPlace";
    case State::Done:         return "Done";
  }
  return "?";
}

void moveTo(const Pose& p) {
  arm.setAll(p.base, p.shoulder, p.elbow, p.wrist, p.wristRot, p.gripper);
}

void enter(State next) {
  state = next;
  stateStart = millis();
  Serial.print(F("block "));
  Serial.print(block);
  Serial.print(F(" -> "));
  Serial.println(stateName(next));

  // Entry action: what to do once, when we arrive in this state
  switch (next) {
    case State::GoSafe:       moveTo(SAFE); break;
    case State::ApproachPick: moveTo(ABOVE_PICK); break;
    case State::DescendPick:  moveTo(AT_PICK); break;
    case State::Grab:         arm.setTarget(MyBraccio::GRIPPER, GRIP_CLOSED); break;
    case State::LiftPick:     moveTo({ABOVE_PICK.base, ABOVE_PICK.shoulder, ABOVE_PICK.elbow,
                                      ABOVE_PICK.wrist, ABOVE_PICK.wristRot, GRIP_CLOSED}); break;
    case State::Carry:        moveTo(ABOVE_PLACE); break;
    case State::DescendPlace: moveTo(AT_PLACE[block]); break;
    case State::Release:      arm.setTarget(MyBraccio::GRIPPER, GRIP_OPEN); break;
    case State::LiftPlace:    moveTo({ABOVE_PLACE.base, ABOVE_PLACE.shoulder, ABOVE_PLACE.elbow,
                                      ABOVE_PLACE.wrist, ABOVE_PLACE.wristRot, GRIP_OPEN}); break;
    case State::Done:         moveTo(SAFE); break;
  }
}

// A state is finished when the arm has stopped AND a short settle time has passed.
bool arrived(unsigned long settleMs = 300) {
  return !arm.isMoving() && millis() - stateStart >= settleMs;
}

void setup() {
  Serial.begin(9600);
  arm.begin();
  arm.setSpeedAll(2);
  arm.setSpeed(MyBraccio::GRIPPER, 1);   // grip gently
  enter(State::GoSafe);
}

void loop() {
  arm.update();   // non-blocking: the state machine below never waits

  switch (state) {
    case State::GoSafe:       if (arrived()) enter(State::ApproachPick); break;
    case State::ApproachPick: if (arrived()) enter(State::DescendPick); break;
    case State::DescendPick:  if (arrived()) enter(State::Grab); break;
    case State::Grab:         if (arrived(500)) enter(State::LiftPick); break;
    case State::LiftPick:     if (arrived()) enter(State::Carry); break;
    case State::Carry:        if (arrived()) enter(State::DescendPlace); break;
    case State::DescendPlace: if (arrived()) enter(State::Release); break;
    case State::Release:      if (arrived(500)) enter(State::LiftPlace); break;
    case State::LiftPlace:
      if (arrived()) {
        block++;
        enter(block < NUM_BLOCKS ? State::ApproachPick : State::Done);
      }
      break;
    case State::Done:
      break;   // finished: hold the safe pose
  }
}
