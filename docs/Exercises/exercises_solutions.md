# Practice Workbook & Solutions

<div class="lesson-meta"><span>✏️ 28 exercises</span><span>🟢 Easy · 🟡 Medium · 🔴 Hard</span></div>

Use this workbook **after** each part to check that the ideas have really stuck. Try every exercise on your own first.
Struggling for 15 minutes teaches more than reading a solution in 15 seconds. Every solution compiles with
`g++ -std=c++17 -Wall -Wextra`.

!!! tip "How to use the solutions"
    Solutions are hidden in collapsible boxes. If you're stuck, open the solution, read only the first few lines, close it,
    and try again.

---

## Part 1: C++ Basics { #part-1-c-basics }

**1.1 🟢 Hello, Braccio.** Write a program that prints `Braccio online: 6 joints` using a `const int` for the number 6.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        const int NUM_JOINTS = 6;
        std::cout << "Braccio online: " << NUM_JOINTS << " joints\n";
        return 0;
    }
    ```

**1.2 🟢 uint8_t overflow.** Declare `uint8_t angle = 180;`, add 100, and print it (as a number). Explain the result.

??? success "Solution"
    ```cpp
    #include <cstdint>
    #include <iostream>

    int main() {
        uint8_t angle = 180;
        angle = angle + 100;                               // 280 doesn't fit in 0..255
        std::cout << static_cast<int>(angle) << '\n';      // 24
        return 0;
    }
    ```
    `uint8_t` holds 0–255. 280 wraps around: 280 − 256 = **24**. A servo would receive 24° instead of "180 + 100".

**1.3 🟢 Grade the pose.** Given `int shoulder = 170;`, print `"OK"` if it's within 15–165, `"LOW"` if below, and `"HIGH"` if above.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        int shoulder = 170;
        if (shoulder < 15) {
            std::cout << "LOW\n";
        } else if (shoulder > 165) {
            std::cout << "HIGH\n";
        } else {
            std::cout << "OK\n";
        }
        return 0;
    }
    ```

**1.4 🟡 Sweep table.** Print a table of base angles from 0 to 180 in steps of 30, with the servo pulse width
`544 + angle * (2400 - 544) / 180` in microseconds.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        for (int angle = 0; angle <= 180; angle += 30) {
            long us = 544 + static_cast<long>(angle) * (2400 - 544) / 180;
            std::cout << angle << " deg\t" << us << " us\n";
        }
        return 0;
    }
    ```

**1.5 🟡 Shortest turn.** Write `int shortestTurn(int from, int to)` that returns the signed number of degrees to rotate
(positive or negative). Test it with 45 → 135 (+90) and 150 → 30 (−120).

??? success "Solution"
    ```cpp
    #include <iostream>

    int shortestTurn(int from, int to) {
        return to - from;   // the base can't wrap past 0/180, so the direct difference is the only path
    }

    int main() {
        std::cout << shortestTurn(45, 135) << ' ' << shortestTurn(150, 30) << '\n';   // 90 -120
        return 0;
    }
    ```
    For a joint that *could* spin a full 360°, you'd wrap the result into −180…+180. The Braccio's servos can't, so
    the plain difference is correct: a good reminder to model the real hardware.

**1.6 🟡 Header split.** Put `clampAngle` and `shortestTurn` in `arm_math.h` / `arm_math.cpp` with an include guard, and
call them from `main.cpp`.

??? success "Solution"
    ```cpp
    // FILE: arm_math.h
    #ifndef ARM_MATH_H
    #define ARM_MATH_H
    int clampAngle(int angle, int lo, int hi);
    int shortestTurn(int from, int to);
    #endif
    // FILE: arm_math.cpp
    #include "arm_math.h"
    int clampAngle(int angle, int lo, int hi) { return angle < lo ? lo : (angle > hi ? hi : angle); }
    int shortestTurn(int from, int to) { return to - from; }
    // FILE: main.cpp
    #include <iostream>
    #include "arm_math.h"
    int main() {
        std::cout << clampAngle(200, 15, 165) << ' ' << shortestTurn(90, 45) << '\n';   // 165 -45
        return 0;
    }
    ```
    Build: `g++ main.cpp arm_math.cpp -o arm_math`

**1.7 🔴 Macro detective.** The author expected `r` to be 25. What value does it really get, why, and how do you fix it properly?
```cpp
#define HALF(x) x / 2
int r = HALF(40 + 10);
```

??? success "Solution"
    It expands to `40 + 10 / 2` = 40 + 5 = **45**. Division happens before addition, and the macro has no parentheses
    to prevent it. Always expand macros by hand to check. Fix: `#define HALF(x) ((x) / 2)`, or better:
    ```cpp
    #include <iostream>

    constexpr int half(int x) { return x / 2; }

    int main() {
        std::cout << half(40 + 10) << '\n';   // 25
        return 0;
    }
    ```

---

## Part 2: Classes { #part-2-classes }

**2.1 🟢 A `Led` class.** Class with private `bool _on`, methods `on()`, `off()`, `toggle()`, and `bool isOn() const`.

??? success "Solution"
    ```cpp
    #include <iostream>

    class Led {
    public:
        void on() { _on = true; }
        void off() { _on = false; }
        void toggle() { _on = !_on; }
        bool isOn() const { return _on; }
    private:
        bool _on = false;
    };

    int main() {
        Led status;
        status.toggle();
        std::cout << std::boolalpha << status.isOn() << '\n';   // true
        return 0;
    }
    ```

**2.2 🟢 Constructor with defaults.** Write `class Joint` whose constructor takes `(const char* name, int minA = 0, int maxA = 180)`
and initialises everything in the member initializer list.

??? success "Solution"
    ```cpp
    #include <iostream>

    class Joint {
    public:
        explicit Joint(const char* name, int minA = 0, int maxA = 180)
            : _name(name), _min(minA), _max(maxA), _angle((minA + maxA) / 2) {}
        void print() const { std::cout << _name << " " << _min << ".." << _max << " at " << _angle << '\n'; }
    private:
        const char* _name;
        int _min, _max, _angle;
    };

    int main() {
        Joint base("base");
        Joint gripper("gripper", 10, 73);
        base.print();      // base 0..180 at 90
        gripper.print();   // gripper 10..73 at 41
        return 0;
    }
    ```

**2.3 🟡 Invariant keeper.** Extend `Joint` with `bool setAngle(int)` (clamps, returns whether exact) and
`bool setLimits(int, int)` (rejects min > max, re-clamps the angle).

??? success "Solution"
    ```cpp
    #include <iostream>

    class Joint {
    public:
        Joint(int minA, int maxA) : _min(minA), _max(maxA), _angle(minA) {}
        bool setAngle(int a) {
            _angle = a < _min ? _min : (a > _max ? _max : a);
            return _angle == a;
        }
        bool setLimits(int minA, int maxA) {
            if (minA > maxA) return false;
            _min = minA;
            _max = maxA;
            setAngle(_angle);          // keep min <= angle <= max
            return true;
        }
        int angle() const { return _angle; }
    private:
        int _min, _max, _angle;
    };

    int main() {
        Joint j(0, 180);
        j.setAngle(170);
        std::cout << j.setLimits(100, 50) << ' ' << j.angle() << '\n';   // 0 170
        j.setLimits(20, 120);
        std::cout << j.angle() << '\n';                                  // 120
        return 0;
    }
    ```

**2.4 🟡 Const correctness.** Which lines fail to compile, and why?
```cpp
class Gripper {
public:
    int angle() { return _a; }
    void close() { _a = 73; }
private:
    int _a = 10;
};
void show(const Gripper& g) {
    std::cout << g.angle();   // line A
}
```

??? success "Solution"
    **Line A** fails: `g` is a `const Gripper&`, and `angle()` isn't marked `const`, so the compiler must assume it
    might modify `g`. Fix: `int angle() const { return _a; }`.

**2.5 🟡 Static counter.** Give `Joint` a `static int count()` that returns how many joints currently exist
(increment in the constructor, decrement in the destructor).

??? success "Solution"
    ```cpp
    #include <iostream>

    class Joint {
    public:
        Joint() { _alive++; }
        ~Joint() { _alive--; }
        static int count() { return _alive; }
    private:
        static int _alive;
    };
    int Joint::_alive = 0;

    int main() {
        Joint a, b;
        {
            Joint c;
            std::cout << Joint::count() << '\n';   // 3
        }
        std::cout << Joint::count() << '\n';       // 2
        return 0;
    }
    ```

**2.6 🔴 Pose class with operators.** Write `struct Pose` with six ints, `operator==`, and a method
`Pose mirrored() const` that mirrors the base (`180 - base`) and the wrist rotation.

??? success "Solution"
    ```cpp
    #include <iostream>

    struct Pose {
        int base, shoulder, elbow, wrist, wristRot, gripper;
        bool operator==(const Pose& o) const {
            return base == o.base && shoulder == o.shoulder && elbow == o.elbow &&
                   wrist == o.wrist && wristRot == o.wristRot && gripper == o.gripper;
        }
        Pose mirrored() const {
            Pose p = *this;
            p.base = 180 - base;
            p.wristRot = 180 - wristRot;
            return p;
        }
    };

    int main() {
        Pose left = {30, 80, 100, 70, 45, 10};
        Pose right = left.mirrored();
        std::cout << right.base << ' ' << right.wristRot << ' '
                  << (right.mirrored() == left ? "round-trip OK" : "bug") << '\n';   // 150 135 round-trip OK
        return 0;
    }
    ```

**2.7 🔴 Design review.** A classmate's class stores `public: int angles[6];` and has a method `void set(int j, int a) { angles[j] = a; }`.
List four improvements.

??? success "Solution"
    1. Make `angles` **private**, so nobody can bypass `set`.
    2. **Validate `j`** (0–5) and return `bool`.
    3. **Clamp `a`** to per-joint limits stored in the class.
    4. Add `const` getters (`int angle(int j) const`) and name the joints with an `enum` instead of magic numbers.
    Bonus: a constructor that initialises all angles to a known safe pose.

---

## Part 3: Memory { #part-3-memory }

**3.1 🟢 Through a pointer.** Declare `int voltage = 5;` and a pointer to it. Set voltage to 6 **only** through the pointer.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        int voltage = 5;
        int* p = &voltage;
        *p = 6;
        std::cout << voltage << '\n';   // 6
        return 0;
    }
    ```

**3.2 🟢 Reference swap.** Write `void swapJoints(int& a, int& b)`.

??? success "Solution"
    ```cpp
    #include <iostream>

    void swapJoints(int& a, int& b) {
        int t = a;
        a = b;
        b = t;
    }

    int main() {
        int base = 30, wrist = 120;
        swapJoints(base, wrist);
        std::cout << base << ' ' << wrist << '\n';   // 120 30
        return 0;
    }
    ```

**3.3 🟡 Const-reference printing.** Write `struct JointLimits { int min, max; };` and `void printLimits(const JointLimits& l)`.
Explain why `const&` is the right choice.

??? success "Solution"
    ```cpp
    #include <iostream>

    struct JointLimits { int min, max; };

    void printLimits(const JointLimits& l) {
        std::cout << l.min << ".." << l.max << '\n';
    }

    int main() {
        JointLimits shoulder = {15, 165};
        printLimits(shoulder);
        return 0;
    }
    ```
    `&` avoids copying the struct, and `const` guarantees `printLimits` can't change it. You get the speed of a pointer
    with the safety of pass-by-value. (For a tiny struct like this the copy is cheap, but the habit matters for big objects.)

**3.4 🟡 Array statistics.** Given `int readings[] = {88, 91, 90, 93, 89, 90};`, compute the minimum, maximum and average
using `sizeof` to find the length.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        int readings[] = {88, 91, 90, 93, 89, 90};
        const int n = sizeof(readings) / sizeof(readings[0]);
        int lo = readings[0], hi = readings[0];
        long sum = 0;
        for (int i = 0; i < n; i++) {
            if (readings[i] < lo) lo = readings[i];
            if (readings[i] > hi) hi = readings[i];
            sum += readings[i];
        }
        std::cout << lo << ' ' << hi << ' ' << static_cast<double>(sum) / n << '\n';   // 88 93 90.1667
        return 0;
    }
    ```

**3.5 🟡 Find the bug.**
```cpp
int limits[6] = {180, 165, 180, 180, 180, 73};
for (int i = 1; i <= 6; i++) Serial.println(limits[i]);
```

??? success "Solution"
    The loop starts at index **1** (skipping the base) and ends at **6**, which is one past the end of the array. It prints
    garbage, or worse. It should be `for (int i = 0; i < 6; i++)`.

**3.6 🔴 Heap to static.** Rewrite this without `new`/`delete`, for a microcontroller:
```cpp
int* recording = new int[count * 6];
// ... fill and play ...
delete[] recording;
```

??? success "Solution"
    ```cpp
    #include <iostream>

    const int MAX_POSES = 20;
    const int NUM_JOINTS = 6;
    int recording[MAX_POSES][NUM_JOINTS];   // reserved at compile time
    int poseCount = 0;

    bool addPose(const int pose[NUM_JOINTS]) {
        if (poseCount >= MAX_POSES) return false;   // full: refuse instead of crashing
        for (int j = 0; j < NUM_JOINTS; j++) recording[poseCount][j] = pose[j];
        poseCount++;
        return true;
    }

    int main() {
        int home[NUM_JOINTS] = {90, 90, 90, 90, 90, 50};
        addPose(home);
        std::cout << poseCount << " pose(s), first base = " << recording[0][0] << '\n';
        return 0;
    }
    ```
    The memory cost is visible at compile time, there's no fragmentation, and "full" is handled explicitly.

**3.7 🔴 Pointer walk.** Using only a pointer (no `[]`), print every character of `const char* cmd = "MOVE";` on its own line,
and count them.

??? success "Solution"
    ```cpp
    #include <iostream>

    int main() {
        const char* cmd = "MOVE";
        int count = 0;
        for (const char* p = cmd; *p != '\0'; p++) {
            std::cout << *p << '\n';
            count++;
        }
        std::cout << count << " characters\n";   // 4
        return 0;
    }
    ```

---

## Part 4: Libraries { #part-4-libraries }

**4.1 🟢 Properties.** Write a `library.properties` for `GripperPlus` 0.1.0 by you, AVR only, depending on `Servo`.

??? success "Solution"
    ```ini
    name=GripperPlus
    version=0.1.0
    author=Your Name <you@example.com>
    maintainer=Your Name <you@example.com>
    sentence=Soft grip and grip-force presets for the Braccio gripper.
    paragraph=
    category=Device Control
    url=https://github.com/you/GripperPlus
    architectures=avr
    depends=Servo
    ```

**4.2 🟢 Where does it go?** In a library, where do these belong: the class definition, method bodies, an example sketch,
the version number?

??? success "Solution"
    Class definition → `src/Name.h` · method bodies → `src/Name.cpp` · example → `examples/Example/Example.ino` ·
    version → `library.properties`.

**4.3 🟡 Non-blocking timer class.** Write `class Every` with a constructor taking a period in ms and `bool ready(unsigned long now)`
that returns `true` once per period (rollover-safe). Test it with a fake clock.

??? success "Solution"
    ```cpp
    #include <iostream>

    class Every {
    public:
        explicit Every(unsigned long periodMs) : _period(periodMs), _last(0) {}
        bool ready(unsigned long now) {
            if (now - _last < _period) return false;   // subtraction: rollover-safe
            _last = now;
            return true;
        }
    private:
        unsigned long _period, _last;
    };

    int main() {
        Every blink(250);
        int fired = 0;
        for (unsigned long t = 0; t <= 1000; t++) {
            if (blink.ready(t)) fired++;
        }
        std::cout << "fired " << fired << " times in 1 s\n";   // 4 (at 250, 500, 750, 1000)
        return 0;
    }
    ```

**4.4 🟡 Spot the review issues.**
```cpp
bool Braccio::setOneAbsolute(int joint, int value) {
  int out = constrain(value, _jointMin[joint], _jointMax[joint]);
  _targetJointPositions[joint] = out;
  return joint == out;
}
```

??? success "Solution"
    1. `return joint == out;` should be `return value == out;` (Lesson 18, bug 1).
    2. `joint` isn't validated: out-of-range values index outside the arrays (bug 4).

**4.5 🟡 Fake it.** Write a minimal fake `Servo` class that records the last angle written, and use it to test a
function `void closeGripper(Servo& s)` that writes 73.

??? success "Solution"
    ```cpp
    #include <iostream>

    class Servo {                  // a fake: records instead of moving
    public:
        void write(int a) { lastAngle = a; writes++; }
        int lastAngle = -1;
        int writes = 0;
    };

    void closeGripper(Servo& s) { s.write(73); }

    int main() {
        Servo fake;
        closeGripper(fake);
        bool ok = fake.lastAngle == 73 && fake.writes == 1;
        std::cout << (ok ? "PASS" : "FAIL") << '\n';
        return ok ? 0 : 1;
    }
    ```

**4.6 🔴 Overshoot-free stepping.** Write `int stepToward(int current, int target, int maxStep)` and test that it never
overshoots for every combination of current 0–180, target 0–180 and maxStep 1–10.

??? success "Solution"
    ```cpp
    #include <iostream>

    int stepToward(int current, int target, int maxStep) {
        int d = target - current;
        if (d > maxStep) d = maxStep;
        if (d < -maxStep) d = -maxStep;
        return current + d;
    }

    int main() {
        long checks = 0;
        for (int c = 0; c <= 180; c++)
            for (int t = 0; t <= 180; t++)
                for (int s = 1; s <= 10; s++) {
                    int n = stepToward(c, t, s);
                    bool between = (c <= t) ? (n >= c && n <= t) : (n <= c && n >= t);
                    if (!between) { std::cout << "FAIL " << c << ' ' << t << ' ' << s << '\n'; return 1; }
                    checks++;
                }
        std::cout << checks << " cases, none overshoot\n";   // 327610 cases
        return 0;
    }
    ```
    Testing **every** case is only possible because the input space is small. It's a great technique for embedded code.

**4.7 🔴 Design challenge.** Sketch (on paper) a `Gripper` class for MyBraccio with *soft grip*: close slowly and stop
early when the servo stops making progress. What would you need to measure, and what API would you offer?

??? success "Discussion"
    The hobby servo gives no position feedback, so you'd need extra hardware: a current sensor (a stalled servo draws
    more current), a force-sensitive resistor on the finger, or a servo with a feedback wire. A possible API:
    `bool gripSoft(uint8_t maxAngle, uint16_t forceLimit)`, `bool isHolding() const`, `void release()`, with a non-blocking
    `update()` that closes one degree at a time and stops when the sensor passes the limit.

---

Want more? Every lesson ends with its own exercises, and every project has extension challenges.
