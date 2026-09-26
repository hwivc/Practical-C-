# Lesson 07: The Preprocessor

<div class="lesson-meta"><span>⏱ 60 min</span><span>🎯 Intermediate</span><span>🧩 Prerequisite: Lesson 06</span></div>

!!! abstract "What you'll learn"
    - Every preprocessor directive: `#include`, `#define`, `#undef`, `#if`, `#ifdef`, `#ifndef`, `#else`, `#endif`, `#error`
    - Object-like and function-like **macros**, and their dangerous traps
    - Why BraccioV2's `#define ELBOW 2` can break *your* sketch
    - **Conditional compilation**: debug builds and a "dry run" safety mode for the arm
    - Modern replacements: `constexpr`, `enum class`, `inline` functions

---

## The preprocessor is a text editor

Before the compiler sees a single line of your code, the **preprocessor** runs through it and follows the lines that start
with `#`. It knows nothing about C++: it does not understand types, scopes or functions. It just **edits text**.

| Directive | What it does |
|---|---|
| `#include <f>` / `"f"` | paste in the contents of file `f` |
| `#define NAME text` | from now on, replace `NAME` with `text` |
| `#undef NAME` | forget `NAME` |
| `#ifdef NAME` / `#ifndef NAME` | keep the following lines only if `NAME` is (not) defined |
| `#if expr` / `#elif` / `#else` / `#endif` | keep lines depending on a constant expression |
| `#error "msg"` | stop the build with a message |
| `#pragma ...` | compiler-specific instructions (like `#pragma once`) |

You can always see what the preprocessor produced with `g++ -E file.cpp`.

---

## Object-like macros

```cpp
#define GRIPPER_CLOSED 73
#define BAUD_RATE 9600

Serial.begin(BAUD_RATE);   // the compiler sees: Serial.begin(9600);
```

BraccioV2 defines its joint names and pins this way:

```cpp title="BraccioV2.h (excerpt)"
#define BASE_ROT 0
#define SHOULDER 1
#define ELBOW 2
#define WRIST 3
#define WRIST_ROT 4
#define GRIPPER 5

#define _BASE_ROT_PIN 11
#define _SHOULDER_PIN 10
```

### The trap: macros ignore scope

Because the preprocessor just replaces text, a macro affects **every** later occurrence of that word, *including your own
variables*. Suppose you include BraccioV2 and then write, reasonably:

```cpp title="macro_clash.cpp"
#include <iostream>
#define ELBOW 2          // what BraccioV2.h does

// compile error expected: the preprocessor turns this into  int 2 = 9;
int ELBOW = 9;           // "my elbow servo pin"

int main() {
    std::cout << ELBOW << '\n';
    return 0;
}
```

```text
error: expected unqualified-id before numeric constant
    2 | #define ELBOW 2
      |               ^
note: in expansion of macro 'ELBOW'
    5 | int ELBOW = 9;
```

The error message is baffling if you don't know that `ELBOW` was a macro. A `const int` or an `enum` would have
been scoped and type-checked instead.

---

## Function-like macros and their traps

A macro can take parameters:

```cpp
#define SQUARE(x) x * x
```

It looks like a function, but it's still **text replacement**, and that causes two classic bugs:

```cpp title="macro_traps.cpp"
#include <iostream>

#define SQUARE_BAD(x)  x * x
#define SQUARE(x)      ((x) * (x))
#define MAX(a, b)      ((a) > (b) ? (a) : (b))

int counter = 0;
int nextReading() { return ++counter * 10; }   // returns 10, 20, 30, ...

int main() {
    // Trap 1: operator precedence
    std::cout << SQUARE_BAD(2 + 3) << '\n';   // becomes 2 + 3 * 2 + 3 = 11, not 25!
    std::cout << SQUARE(2 + 3) << '\n';       // ((2 + 3) * (2 + 3)) = 25

    // Trap 2: arguments are evaluated more than once
    int m = MAX(nextReading(), 5);            // ((nextReading()) > (5) ? (nextReading()) : (5))
    std::cout << "max = " << m << ", nextReading() was called " << counter << " times\n";   // max = 20, called 2 times!
    return 0;
}
```

**Rules if you must write one:** put parentheses around every parameter *and* around the whole expression, and never
pass expressions with side effects (`i++`, function calls) to a macro.

!!! bug "Arduino's `constrain` is a macro"
    On AVR boards, `constrain`, `min`, `max` and `abs` are macros in `Arduino.h`. So this reads the sensor **up to three times**:

    ```cpp
    int angle = constrain(analogRead(A0) / 5, 0, 180);
    ```

    Store the value first: `int raw = analogRead(A0) / 5; int angle = constrain(raw, 0, 180);`

### The modern replacement: `inline` functions (and `constexpr`)

```cpp
inline int square(int x) { return x * x; }                   // type-checked, evaluates x once
constexpr int clampAngle(int a, int lo, int hi) {            // can even run at compile time
    return a < lo ? lo : (a > hi ? hi : a);
}
```

These are just as fast as macros, because the compiler inlines small functions, but they obey types and scope, and
their arguments are evaluated exactly once.

---

## Conditional compilation

Conditional compilation keeps or removes whole blocks of code **before compiling**. This is where the preprocessor
really shines.

### Debug builds

```cpp
#define DEBUG

#ifdef DEBUG
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINTLN(x)          // expands to nothing
#endif
```

With `DEBUG` defined, `DEBUG_PRINTLN("moving");` prints. Comment out `#define DEBUG` and every debug line **disappears
from the program**. It takes no flash, no RAM and no time.

### Different boards

The Arduino toolchain defines macros for each board, so one library can support several chips:

```cpp
#if defined(ARDUINO_ARCH_AVR)
  // Arduino UNO, Mega, Nano: 8-bit AVR
#elif defined(ARDUINO_ARCH_SAMD)
  // Arduino Zero, MKR: 32-bit ARM
#else
  #error "This library only supports AVR and SAMD boards"
#endif
```

### Predefined macros

| Macro | Expands to |
|---|---|
| `__FILE__` | the current file name, as a string |
| `__LINE__` | the current line number |
| `__DATE__` / `__TIME__` | when the file was compiled |
| `__cplusplus` | the C++ standard version (e.g. `201703L`) |
| `ARDUINO` | the Arduino IDE version (defined only in Arduino builds) |

```cpp title="predefined.cpp"
#include <iostream>

#define TRACE() std::cout << "[" << __FILE__ << ":" << __LINE__ << "] "

int main() {
    TRACE() << "starting arm test\n";
    TRACE() << "C++ version " << __cplusplus << '\n';
#ifdef ARDUINO
    std::cout << "running on Arduino\n";
#else
    std::cout << "running on a PC\n";
#endif
    return 0;
}
```

---

## Replacing BraccioV2's macros with modern C++

Here's how the library's joint names could be written without macros. You'll do exactly this in `MyBraccio` in
[Lesson 19](../Part4_Arduino_Libraries/lesson19_coding_mybraccio.md):

```cpp title="modern_names.cpp"
#include <iostream>
#include <cstdint>

// Scoped, typed joint names: no clashes with user variables called ELBOW
enum class Joint : uint8_t { Base = 0, Shoulder, Elbow, Wrist, WristRot, Gripper };

// Typed, scoped pins
constexpr uint8_t PIN_FOR[6] = {11, 10, 9, 6, 5, 3};

int main() {
    Joint j = Joint::Elbow;
    int ELBOW = 9;   // perfectly fine now: nothing else is called ELBOW
    std::cout << "Elbow index " << static_cast<int>(j)
              << ", pin " << static_cast<int>(PIN_FOR[static_cast<int>(j)])
              << ", my var " << ELBOW << '\n';
    return 0;
}
```

| Old (macro) | Modern replacement | Why better |
|---|---|---|
| `#define ELBOW 2` | `enum class Joint { ..., Elbow, ... }` | scoped, typed, can't clash |
| `#define GLOBAL_MAX 180` | `constexpr int GLOBAL_MAX = 180;` | has a type, obeys scope |
| `#define SQUARE(x) ((x)*(x))` | `inline int square(int x)` | argument evaluated once, type-checked |
| `#ifdef DEBUG` | *(no replacement needed)* | still the right tool for build switches |

---

## :material-robot-industrial: Arm Lab: debug and dry-run switches

!!! arm "Arm Lab 07"
    Two build switches control this sketch. `DEBUG` prints every command with its line number. `DRY_RUN` prints commands
    **without moving the arm**, which is a great way to check a new motion sequence before trusting it with the hardware.

```cpp title="L07_debug.ino"
--8<-- "examples/arm_labs/L07_debug/L07_debug.ino"
```

**Try this:**

1. Verify with `DEBUG` on, then off. Compare the "Sketch uses … bytes" numbers.
2. Enable `DRY_RUN` and upload. The Serial Monitor shows the plan, but the arm stays still.
3. Add `#error "Remove DRY_RUN before the demo!"` inside an `#ifdef DRY_RUN` block. What happens when you Verify?

??? success "Answers"
    1. With `DEBUG` off, the sketch is smaller in flash **and** RAM: every string literal and print call is gone.
    2. The arm is never started (`arm.begin()` is skipped), so it doesn't move.
    3. The build **stops** with your message. It's a handy guard against shipping test settings.

---

## Exercises

**1. Predict the output.**
```cpp
#define DOUBLE(x) x + x
int a = DOUBLE(5) * 2;
```

**2. Fix the macro.** Rewrite `#define DEG_TO_RAD(d) d * 3.14159 / 180` so that `DEG_TO_RAD(90 + 90)` gives π.
Then write a `constexpr` function that does the same thing.

**3. Board check.** Write a header `board_check.h` that stops the build with `#error` unless it's compiled for an AVR board.

??? success "Solution 1"
    It expands to `5 + 5 * 2` = **15**, not 20. The fix is `#define DOUBLE(x) ((x) + (x))`.

??? success "Solution 2"
    ```cpp
    #define DEG_TO_RAD(d) ((d) * 3.14159 / 180)

    constexpr double degToRad(double d) { return d * 3.14159265358979 / 180.0; }
    ```
    Without parentheses, `90 + 90 * 3.14159 / 180` = 91.57. Arduino already provides `DEG_TO_RAD` as a *constant*
    (0.0174533) you multiply by: `radians = degrees * DEG_TO_RAD`.

??? success "Solution 3"
    ```cpp
    #ifndef BOARD_CHECK_H
    #define BOARD_CHECK_H

    #if !defined(ARDUINO_ARCH_AVR)
      #error "This sketch needs an AVR board such as the Arduino UNO."
    #endif

    #endif
    ```

---

## Recap

- The preprocessor edits **text** before compilation and knows nothing about C++.
- Macros ignore scope and types: `#define ELBOW 2` can clash with your variables.
- Function-like macros need parentheses everywhere and may evaluate arguments **more than once**.
- Prefer `constexpr`, `enum class` and `inline` functions for constants and small helpers.
- Conditional compilation (`#ifdef DEBUG`, `DRY_RUN`, board checks) is where the preprocessor is the right tool.

🎉 **You've finished Part 1!** Test yourself with the [Part 1 workbook](../Exercises/exercises_solutions.md#part-1-c-basics).

## Further reading

- [LearnCpp: Introduction to the preprocessor](https://www.learncpp.com/cpp-tutorial/introduction-to-the-preprocessor/)
- [cppreference: Preprocessor](https://en.cppreference.com/w/cpp/preprocessor)
- [C++ Core Guidelines: ES.31, don't use macros for constants or functions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-macros2)
