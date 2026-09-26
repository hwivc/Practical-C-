# BraccioV2 & MyBraccio API Reference

A quick reference for both libraries used in this course. For how they work inside, see
[Lesson 18](../Part4_Arduino_Libraries/lesson18_bracciov2_deep_dive.md) and [Lesson 19](../Part4_Arduino_Libraries/lesson19_coding_mybraccio.md).

---

## Joints, pins and limits

| Joint | BraccioV2 name | MyBraccio name | Pin | Default range | Upright |
|---|---|---|:-:|:-:|:-:|
| M1 base | `BASE_ROT` (0) | `MyBraccio::BASE` | 11 | 0–180 | 90 |
| M2 shoulder | `SHOULDER` (1) | `MyBraccio::SHOULDER` | 10 | 15–165 | 90 |
| M3 elbow | `ELBOW` (2) | `MyBraccio::ELBOW` | 9 | 0–180 | 90 |
| M4 wrist | `WRIST` (3) | `MyBraccio::WRIST` | 6 | 0–180 | 90 |
| M5 wrist rotation | `WRIST_ROT` (4) | `MyBraccio::WRIST_ROT` | 5 | 0–180 | 90 |
| M6 gripper | `GRIPPER` (5) | `MyBraccio::GRIPPER` | 3 | 10 (open) – 73 (closed) | 50 |
| Soft-start power | `SOFT_START_PIN` | (internal) | 12 | — | — |

---

## BraccioV2

```cpp
#include <BraccioV2.h>   // install "BraccioV2" from the Library Manager
Braccio arm;
```

### Start-up

| Method | Description |
|---|---|
| `void begin()` | Attach servos, move to the calibrated centres, soft-start power (~6 s). |
| `void begin(bool defaultPos)` | `false`: don't move to the centres. ⚠️ Call `setAllNow(...)` **immediately** afterwards, or the first move starts from 0° (bug 5). |

### Targets (non-blocking; motion happens in `update()`)

| Method | Returns | Description |
|---|---|---|
| `bool setOneAbsolute(int joint, int value)` | ⚠️ unreliable (bug 1) | Set one joint's target, clamped to its limits. |
| `bool setOneRelative(int joint, int value)` | `true` if not clamped | Add `value` to the joint's current **target**. |
| `bool setAllAbsolute(int b, int s, int e, int w, int w_r, int g)` | ⚠️ unreliable | Set all six targets. |
| `bool setAllRelative(int b, int s, int e, int w, int w_r, int g)` | `true` if none clamped | Add to all six targets. |
| `void setAllNow(int b, int s, int e, int w, int w_r, int g)` | — | Write all six **immediately**, with **no clamping**. |

### Motion

| Method | Description |
|---|---|
| `void update()` | Move every joint one `delta` toward its target. Call every ~10 ms. |
| `void safeDelay(int ms)` | Wait `ms` ms, calling `update()` every 10 ms. Max 32 767 ms. |
| `void safeDelay(int ms, int t)` | Same, calling `update()` every `t` ms. |

### Configuration

| Method | Description |
|---|---|
| `void setJointMin(int joint, int value)` | Lower limit (clamped to 0–180; not checked against the max). |
| `void setJointMax(int joint, int value)` | Upper limit (clamped to 0–180). |
| `void setJointCenter(int joint, int offset)` | Calibrated centre, used by `begin()`. Call **before** `begin()`. |
| `int getCenter(int joint)` | Read the calibrated centre. |
| `void setDelta(int joint, int value)` | Degrees per `update()` (default 1). ⚠️ Use values that divide your move distances (bug 2); never ≤ 0. |

### Typical sketch

```cpp
#include <BraccioV2.h>
Braccio arm;

void setup() {
  arm.setJointCenter(SHOULDER, 93);   // calibration first
  arm.begin();
}

void loop() {
  arm.setAllAbsolute(45, 90, 90, 90, 90, 10);
  arm.safeDelay(2000);
  arm.setAllAbsolute(135, 90, 90, 90, 90, 73);
  arm.safeDelay(2000);
}
```

---

## MyBraccio

```cpp
#include <MyBraccio.h>   // copy examples/libraries/MyBraccio into Documents/Arduino/libraries
MyBraccio arm;
```

### Start-up

| Method | Description |
|---|---|
| `void begin()` | Command the home pose, then soft-start power (~6 s). |
| `void begin(const int startPose[6])` | Start in any pose (clamped). |

### Targets (non-blocking)

| Method | Returns | Description |
|---|---|---|
| `bool setTarget(uint8_t joint, int angle)` | `true` if used unchanged | One joint; clamped; invalid joint → `false`. |
| `bool setAll(int b, int s, int e, int w, int wr, int g)` | `true` if none clamped | All six joints. |
| `bool setAll(const int pose[6])` | same | All six from an array. |
| `bool nudge(uint8_t joint, int degrees)` | `true` if not clamped | Relative to the current target. |
| `void stop()` | — | Set every target to the current angle (freeze). |

### Motion

| Method | Description |
|---|---|
| `void update()` | Call as often as you like: steps only every `stepInterval` ms (default 10). Returns immediately otherwise. |
| `void waitUntilStopped(unsigned long timeoutMs = 10000)` | **Blocks** until no joint is moving, or the timeout passes. |
| `void delayWhileMoving(unsigned long ms)` | **Blocks** for `ms`, keeping the arm moving. Rollover-safe. |

### Configuration

| Method | Returns | Description |
|---|---|---|
| `bool setLimits(uint8_t joint, int min, int max)` | `false` if invalid | Rejects min > max or values outside 0–180. Re-clamps the target. |
| `bool setSpeed(uint8_t joint, uint8_t degreesPerStep)` | `false` if not 1–10 | Speed of one joint. |
| `void setSpeedAll(uint8_t degreesPerStep)` | — | Speed of every joint. |
| `bool setStepInterval(uint16_t ms)` | `false` if not 5–100 | Time between steps. |

### Queries (all `const`)

| Method | Returns |
|---|---|
| `int angle(uint8_t joint)` | Current commanded angle (−1 for an invalid joint) |
| `int target(uint8_t joint)` | Target angle |
| `int minAngle(uint8_t joint)`, `int maxAngle(uint8_t joint)` | Limits |
| `bool isMoving()` / `bool isMoving(uint8_t joint)` | Whether any joint / this joint is still moving |
| `bool started()` | Whether `begin()` has run |

### Typical sketch

```cpp
#include <MyBraccio.h>
MyBraccio arm;

void setup() {
  arm.begin();
  arm.setSpeedAll(2);
}

void loop() {
  arm.update();                         // never blocks
  if (!arm.isMoving()) {
    static bool left = false;
    left = !left;
    arm.setAll(left ? 45 : 135, 90, 90, 90, 90, left ? 10 : 73);
  }
  // ...read sensors, serial commands, buttons here...
}
```

---

## Official Arduino Braccio library (for comparison)

```cpp
#include <Braccio.h>
#include <Servo.h>
Servo base, shoulder, elbow, wrist_rot, wrist_ver, gripper;   // the library expects these global names

void setup() {
  Braccio.begin();                                  // moves to the safety pose
}

void loop() {
  // stepDelay 10-30 ms, then M1..M6
  Braccio.ServoMovement(20, 90, 45, 180, 180, 90, 10);
}
```

`ServoMovement` **blocks** until all joints arrive. See the
[official guide](https://docs.arduino.cc/retired/getting-started-guides/Braccio/) and
[source](https://github.com/arduino-libraries/Braccio).
