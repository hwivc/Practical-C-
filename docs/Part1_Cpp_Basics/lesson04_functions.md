# Lesson 04: Functions

<div class="lesson-meta"><span>⏱ 90 min</span><span>🎯 Beginner</span><span>🧩 Prerequisite: Lesson 03</span></div>

!!! abstract "What you'll learn"
    - Why functions make robot code shorter, safer and readable
    - Parameters, arguments and return values
    - **Declarations** (prototypes) vs **definitions**
    - Pass-by-value, default arguments and **overloading** (used by BraccioV2's `begin()`)
    - `static` local variables that remember values between calls
    - How to turn a long sketch into a readable "story" of named actions

---

## Why functions?

Look at the *find directions* sketch from [Lesson 00](../hardware/first_move.md): the same four lines, copied six times.
If you find a bug in one copy, you have to fix it in all six. A **function** gives a block of code a name so you can
use it again and again:

```cpp
void closeGripper() {
    arm.setOneAbsolute(GRIPPER, 73);
    arm.safeDelay(600);
}
```

Now `closeGripper();` anywhere in the program does the job. The code reads like English, and there is only
**one** place to fix if 73 turns out to be too tight.

---

## Anatomy of a function

```cpp
int clampAngle(int angle, int minAngle, int maxAngle) {   // (1)!
    if (angle < minAngle) return minAngle;                // (2)!
    if (angle > maxAngle) return maxAngle;
    return angle;
}
```

1. **Return type** `int`, **name** `clampAngle`, and three **parameters** (inputs), each with its own type. Together, this line is the function's *signature*.
2. `return` sends a value back to the caller and **ends the function immediately**.

Calling it:

```cpp
int safe = clampAngle(200, 15, 165);   // 200, 15, 165 are the ARGUMENTS; safe == 165
```

| Term | Meaning |
|---|---|
| **Parameter** | the variable in the function's definition (`angle`) |
| **Argument** | the actual value passed in a call (`200`) |
| **Return type** | the type of the value sent back; `void` means "returns nothing" |

```mermaid
sequenceDiagram
    participant loop as loop()
    participant f as clampAngle()
    loop->>f: clampAngle(200, 15, 165)
    Note right of f: angle=200, minAngle=15, maxAngle=165<br/>(copies of the arguments)
    f-->>loop: returns 165
    Note left of loop: safe = 165
```

---

## Declaration vs definition

C++ reads your file **from top to bottom**. You can't call a function the compiler hasn't heard of yet.
There are two fixes:

1. Define the function **above** where it's used, or
2. Put a **declaration** (also called a *prototype*) near the top, and the **definition** anywhere later:

```cpp title="prototypes.cpp"
#include <iostream>

// Declarations: "these functions exist; here's how to call them"
int clampAngle(int angle, int minAngle, int maxAngle);
void printJoint(const char* name, int angle);

int main() {
    printJoint("shoulder", clampAngle(200, 15, 165));
    printJoint("gripper",  clampAngle(-5, 10, 73));
    return 0;
}

// Definitions: the actual code
int clampAngle(int angle, int minAngle, int maxAngle) {
    if (angle < minAngle) return minAngle;
    if (angle > maxAngle) return maxAngle;
    return angle;
}

void printJoint(const char* name, int angle) {
    std::cout << name << " -> " << angle << " deg\n";
}
```

A declaration ends with `;` and has no body. This separation is the foundation of **header files**
([Lesson 05](lesson05_headers.md)): the header holds the declarations, a `.cpp` file holds the definitions.

!!! info "The Arduino IDE cheats for you"
    In an `.ino` file, the IDE automatically generates prototypes for your functions before compiling, which is why
    sketches work in any order. In normal `.cpp` files (and in libraries) you must do it yourself. In this course
    we define helper functions above `setup()` so the code works everywhere.

---

## Pass-by-value: functions get copies

By default, arguments are **copied** into the parameters. Changing a parameter doesn't change the caller's variable:

```cpp title="by_value.cpp"
#include <iostream>

void tryToMove(int angle) {
    angle = 180;                 // changes the COPY only
}

int main() {
    int shoulder = 90;
    tryToMove(shoulder);
    std::cout << "shoulder is still " << shoulder << '\n';   // 90
    return 0;
}
```

This is usually what you want: a function can't mess up your variables by accident. When you *do* want a function
to change the caller's variable, or want to avoid copying something large, you'll use **references**
([Lesson 13](../Part3_Memory/lesson13_references.md)).

---

## Default arguments

A parameter can have a default value, which is used when the caller leaves it out:

```cpp
void moveTo(int base, int shoulder, int elbow, int wrist, int wristRot, int gripper,
            unsigned long waitMs = 1500);

moveTo(90, 90, 90, 90, 90, 50);          // waits 1500 ms
moveTo(90, 90, 90, 90, 90, 50, 3000);    // waits 3000 ms
```

Defaults must come **last** in the parameter list.

## Overloading: same name, different parameters

C++ lets several functions share a name if their parameter lists differ. The compiler picks the right one from the
arguments. BraccioV2 does this:

```cpp
void begin();                 // start and move to the default pose
void begin(bool defaultPos);  // start, and choose whether to move

void safeDelay(int ms);         // update every 10 ms
void safeDelay(int ms, int t);  // update every t ms
```

```cpp title="overload.cpp"
#include <iostream>

void report(int angle)               { std::cout << "angle " << angle << '\n'; }
void report(double radians)          { std::cout << "radians " << radians << '\n'; }
void report(const char* joint, int a){ std::cout << joint << " at " << a << '\n'; }

int main() {
    report(90);              // calls report(int)
    report(1.57);            // calls report(double)
    report("elbow", 45);     // calls report(const char*, int)
    return 0;
}
```

---

## `static` local variables

A normal local variable is created fresh on every call. A **`static` local** is created once and **remembers** its
value between calls, but it's still only visible inside its function (unlike a global):

```cpp title="static_local.cpp"
#include <iostream>

int nextMoveId() {
    static int counter = 0;   // initialised only the first time
    counter++;
    return counter;
}

int main() {
    std::cout << nextMoveId() << ' ' << nextMoveId() << ' ' << nextMoveId() << '\n';   // 1 2 3
    return 0;
}
```

---

## Good function design

| ✅ Do | ❌ Avoid |
|---|---|
| One job per function (`openGripper`, `goHome`) | A 200-line `doEverything()` |
| Verb names: `moveTo`, `isInRange`, `readSensor` | Vague names: `stuff`, `func2` |
| Return `bool` from checks: `bool isSafe(int a)` | Printing inside a function that should just compute |
| Name your numbers: `GRIPPER_CLOSED` | Magic numbers like `73` scattered everywhere |
| Keep functions under ~30 lines | Deep nesting (4+ levels of `if` / `for`) |

---

## :material-robot-industrial: Arm Lab: a readable pick-and-place

!!! arm "Arm Lab 04"
    The whole `loop()` now reads like a recipe: *move over the spot → close gripper → carry → open gripper → wave*.
    Look at the default argument on `moveTo` and the `static` counter inside `wave`.

```cpp title="L04_functions.ino"
--8<-- "examples/arm_labs/L04_functions/L04_functions.ino"
```

**Try this:**

1. Write `void goHome()` that moves to the upright pose (all 90, gripper 50) and use it at the end of `loop()`.
2. Write `bool isGripperClosed(int angle)` that returns `true` when `angle >= 60`.
3. Add an overload `void wave()` with no parameters that waves twice by calling `wave(2)`.

---

## Common mistakes

| Mistake | Symptom |
|---|---|
| Forgetting `return` in a non-`void` function | Garbage value returned; `-Wall` warns *control reaches end of non-void function* |
| Calling before declaring (in `.cpp` files) | `'moveTo' was not declared in this scope` |
| Declaration and definition don't match | Linker error: `undefined reference to 'moveTo(int, int)'` |
| Expecting pass-by-value to change the caller's variable | The variable stays unchanged |
| Default argument given in both declaration and definition | `redefinition of default argument`. Put it in the declaration only. |

---

## Exercises

**1. `isInRange`.** Write `bool isInRange(int angle, int minA, int maxA)` and test it with a few values.

**2. `degToRad`.** Write `double degToRad(double degrees)`. Print the radians for 0°, 90° and 180°. (π ≈ 3.14159265)

**3. Speed calculator.** Write `unsigned long moveTimeMs(int from, int to, int delta, int updateMs)` that returns how
long BraccioV2 needs to move a joint from `from` to `to` when each update moves `delta` degrees every `updateMs` ms.
Check: 90 → 150 at delta 1 every 10 ms takes 600 ms.

**4. Arm challenge: shake hands.** Write `void shakeHands(int times)` that moves the elbow up and down by ±15° around its
current position using `setOneRelative`, `times` times.

??? success "Solution 1 & 2"
    ```cpp
    #include <iostream>

    bool isInRange(int angle, int minA, int maxA) {
        return angle >= minA && angle <= maxA;   // the comparison already IS a bool
    }

    double degToRad(double degrees) {
        const double PI_VALUE = 3.14159265358979;
        return degrees * PI_VALUE / 180.0;
    }

    int main() {
        std::cout << std::boolalpha;             // print true/false instead of 1/0
        std::cout << isInRange(90, 15, 165) << ' ' << isInRange(170, 15, 165) << '\n';
        std::cout << degToRad(0) << ' ' << degToRad(90) << ' ' << degToRad(180) << '\n';
        return 0;
    }
    ```

??? success "Solution 3"
    ```cpp
    #include <iostream>

    unsigned long moveTimeMs(int from, int to, int delta, int updateMs) {
        int distance = to - from;
        if (distance < 0) distance = -distance;
        int updates = (distance + delta - 1) / delta;   // round UP: a partial step still costs an update
        return static_cast<unsigned long>(updates) * updateMs;
    }

    int main() {
        std::cout << moveTimeMs(90, 150, 1, 10) << " ms\n";   // 600
        std::cout << moveTimeMs(0, 180, 4, 10) << " ms\n";    // 450
        return 0;
    }
    ```

??? success "Solution 4"
    ```cpp
    void shakeHands(int times) {
      for (int i = 0; i < times; i++) {
        arm.setOneRelative(ELBOW, 15);
        arm.safeDelay(400);
        arm.setOneRelative(ELBOW, -15);
        arm.safeDelay(400);
      }
    }
    ```

---

## Recap

- Functions name a piece of work. Write it once, use it everywhere, fix it in one place.
- Signature = return type + name + parameter list. `return` hands back a value and exits.
- Declare before use: prototypes separate *what* (declaration) from *how* (definition).
- Arguments are **copied** by default. Default arguments go last. Overloads share a name.
- `static` locals remember values between calls.

## Further reading

- [LearnCpp: Functions](https://www.learncpp.com/cpp-tutorial/introduction-to-functions/)
- [LearnCpp: Function overloading](https://www.learncpp.com/cpp-tutorial/introduction-to-function-overloading/)
- [Arduino: Functions](https://docs.arduino.cc/learn/programming/functions/)
