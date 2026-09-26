# Lesson 02: Variables, Types & Constants

<div class="lesson-meta"><span>⏱ 75 min</span><span>🎯 Beginner</span><span>🧩 Prerequisite: Lesson 01</span></div>

!!! abstract "What you'll learn"
    - What a **variable** is, and why every variable in C++ has a **type**
    - The fundamental types, and why `int` is **2 bytes** on the Arduino but 4 on your PC
    - Fixed-width types such as `uint8_t` and `int16_t`, and **overflow**
    - `const`, `constexpr` and `#define` for values that must never change
    - **Scope**: local vs global variables
    - Integer division, casting, and a real bug from the BraccioV2 examples

---

## Why a robot needs variables

To control an arm, a program has to *remember* things: the angle of each joint, which pin each servo is on,
how long to wait, how many times it has waved. A **variable** is a named box in memory that holds a value:

```cpp
int baseAngle = 90;      // create a box called baseAngle, holding 90
baseAngle = 135;         // put a new value in the box
baseAngle = baseAngle + 10;   // read it, add 10, store it back: now 145
```

In C++ every variable has a **type**, fixed when you create it. The type tells the compiler:

1. **How many bytes** the box needs.
2. **What the bits mean**: whole number? decimal? true/false? letter?
3. **What operations are allowed** on it.

This is called **static typing**. It lets the compiler catch mistakes before your code ever reaches the robot.

---

## The fundamental types

| Type | Holds | PC size | **Arduino UNO size** | Range on the UNO | Example |
|---|---|:-:|:-:|---|---|
| `bool` | `true` / `false` | 1 | 1 | — | `bool gripperClosed = false;` |
| `char` | one character | 1 | 1 | −128 … 127 | `char cmd = 'B';` |
| `int` | whole number | 4 | **2** | −32 768 … 32 767 | `int angle = 90;` |
| `unsigned int` | whole number ≥ 0 | 4 | **2** | 0 … 65 535 | `unsigned int count = 0;` |
| `long` | big whole number | 4 or 8 | 4 | ±2 147 483 647 | `long steps = 100000;` |
| `unsigned long` | big whole number ≥ 0 | 4 or 8 | 4 | 0 … 4 294 967 295 | `unsigned long t = millis();` |
| `float` | decimal | 4 | 4 | ~7 significant digits | `float speed = 1.5;` |
| `double` | precise decimal | 8 | **4** (same as float!) | ~7 digits on UNO | `double pi = 3.14159;` |

!!! warning "The size of `int` depends on the chip"
    C++ only promises that `int` is *at least* 16 bits. On your laptop it is 32 bits. On the 8-bit ATmega328P it is
    16 bits, so an `int` on the UNO can't count past **32 767**. That's only 32.7 seconds in milliseconds! Keep this
    in mind for everything that involves **time**.

### Fixed-width types

When the exact size matters, use the types from `<cstdint>` (on Arduino they're available automatically):

| Type | Bits | Range | Good for |
|---|:-:|---|---|
| `uint8_t` | 8 | 0 … 255 | servo angles, pin numbers, joint indexes |
| `int8_t` | 8 | −128 … 127 | small signed offsets |
| `int16_t` / `uint16_t` | 16 | ±32 767 / 0 … 65 535 | sensor readings |
| `int32_t` / `uint32_t` | 32 | ±2.1 billion / 0 … 4.29 billion | timestamps, counters |

Names ending in `_t` are **the same size on every computer**. That makes them ideal for code shared between your PC
and the robot.

```cpp title="sizes.cpp"
#include <iostream>
#include <cstdint>

int main() {
    std::cout << "int:      " << sizeof(int)      << " bytes\n";
    std::cout << "long:     " << sizeof(long)     << " bytes\n";
    std::cout << "uint8_t:  " << sizeof(uint8_t)  << " byte\n";
    std::cout << "int16_t:  " << sizeof(int16_t)  << " bytes\n";
    std::cout << "uint32_t: " << sizeof(uint32_t) << " bytes\n";

    uint8_t angle = 90;
    std::cout << "angle = " << static_cast<int>(angle) << '\n';   // cast so it prints as a number
    return 0;
}
```

!!! note "Why the cast?"
    `uint8_t` is really an `unsigned char`, so `std::cout` would print it as a *character*. The number 90 is the
    letter `Z`! `static_cast<int>(angle)` converts it to an `int` for printing. On the Arduino, `Serial.print` handles
    `uint8_t` correctly without a cast.

---

## Overflow: when the box is too small

What happens if a value no longer fits?

```cpp title="overflow.cpp"
#include <iostream>
#include <cstdint>

int main() {
    uint8_t angle = 250;
    angle = angle + 10;                 // 260 doesn't fit in 0..255
    std::cout << "uint8_t 250 + 10 = " << static_cast<int>(angle) << '\n';   // prints 4

    int16_t ms = 32767;
    ms = ms + 1;                        // doesn't fit in -32768..32767
    std::cout << "int16_t 32767 + 1 = " << ms << '\n';   // prints -32768
    return 0;
}
```

Unsigned types **wrap around** like a car's odometer: 255 + 1 becomes 0. Signed types that overflow give surprising
negative numbers. (Strictly speaking, overflowing a signed `int` is *undefined behaviour* in C++, so anything
can happen. Never rely on it.)

<div class="grid" markdown>

!!! bug "A real bug in the BraccioV2 examples"
    The library's `Advanced_Movement` example contains this line:

    ```cpp
    int endTime = millis() + 3000;
    while (millis() < endTime) { ... }
    ```

    `millis()` returns an `unsigned long` (milliseconds since power-on). Stored in a 16-bit `int`, any value above
    32 767 overflows. About **30 seconds** after power-on, `endTime` becomes negative, the `while` condition is
    immediately false, and the arm stops waiting. The example seems to work at first and then breaks.

!!! success "The fix"
    Always store time in `unsigned long`, and compare *durations*, not end times:

    ```cpp
    unsigned long start = millis();
    while (millis() - start < 3000) { ... }
    ```

    This is correct even when `millis()` itself wraps around after about 49 days.

</div>

---

## Constants: values that must never change

A servo's pin or a joint's limit should never change while the program runs. C++ gives you three ways to say so:

```cpp
#define SHOULDER_PIN 10              // 1. preprocessor text replacement (C style)
const int SHOULDER_MAX = 165;        // 2. typed, read-only variable
constexpr int SHOULDER_MIN = 15;     // 3. guaranteed to be known at compile time
```

| | `#define` | `const` | `constexpr` |
|---|:-:|:-:|:-:|
| Has a type the compiler checks | ✗ | ✓ | ✓ |
| Obeys scope (can be local) | ✗ | ✓ | ✓ |
| Shows up by name in the debugger / errors | ✗ | ✓ | ✓ |
| Usable as an array size | ✓ | ✓ (if initialised with a constant) | ✓ |

**Prefer `const` or `constexpr`.** BraccioV2 uses `#define` for its pins and joint names (it's an older style common in
Arduino code), and in [Lesson 07](lesson07_preprocessor.md) you'll see the problems that causes.

```cpp title="const_error.cpp"
#include <iostream>

const int GRIPPER_MAX = 73;

int main() {
    GRIPPER_MAX = 90;   // compile error: assignment of read-only variable
    std::cout << GRIPPER_MAX << '\n';
    return 0;
}
```

---

## Scope: where a variable lives

- A **local** variable is created inside a block `{ }` and destroyed when the block ends.
- A **global** variable is created outside every function. It exists for the whole program and is visible everywhere below it.

```cpp title="scope.cpp"
#include <iostream>

const int MAX_ANGLE = 180;      // global constant: fine and common
int moveCount = 0;              // global variable: use sparingly

void recordMove() {
    moveCount = moveCount + 1;  // can see the global
}

int main() {
    int target = 120;           // local to main()
    if (target <= MAX_ANGLE) {
        int margin = MAX_ANGLE - target;   // local to this if-block
        std::cout << "Margin: " << margin << '\n';
    }
    // std::cout << margin;     // error: margin no longer exists here
    recordMove();
    recordMove();
    std::cout << "Moves: " << moveCount << '\n';
    return 0;
}
```

!!! tip "In Arduino sketches"
    Variables that must survive **between** calls to `loop()` (a counter, the current mode) must be **global**, or
    `static` (see [Lesson 04](lesson04_functions.md)). A local variable inside `loop()` starts fresh every time.

---

## Arithmetic traps

```cpp title="arithmetic.cpp"
#include <iostream>

int main() {
    int a = 7 / 2;          // integer division: 3, NOT 3.5
    int r = 7 % 2;          // remainder: 1
    double b = 7 / 2;       // still 3! Both sides were ints, so the division was an integer division
    double c = 7.0 / 2;     // 3.5 (one side is a double)
    int deg = 100;
    double half = static_cast<double>(deg) / 3;   // 33.333...
    int rounded = static_cast<int>(half + 0.5);   // 33

    std::cout << a << ' ' << r << ' ' << b << ' ' << c << ' ' << half << ' ' << rounded << '\n';

    int angle = 90;
    angle += 15;   // same as angle = angle + 15   -> 105
    angle -= 5;    // 100
    angle++;       // 101
    std::cout << "angle = " << angle << '\n';
    return 0;
}
```

!!! warning "`int / int` is always an `int`"
    To scale a sensor reading of 0–1023 to 0–180 degrees, `reading / 1023 * 180` gives **0** for every reading below
    1023, because `reading / 1023` is done first in integer maths. Multiply first (`reading * 180L / 1023`, where the
    `L` makes it a `long` so it doesn't overflow) or use `float`. Arduino's `map()` does this for you.

---

## Naming conventions

Good names make code read like a sentence. This course uses:

| Kind | Style | Example |
|---|---|---|
| Variables & functions | `camelCase` | `targetAngle`, `moveToPose()` |
| Constants | `UPPER_SNAKE` or `kCamel` | `GRIPPER_MAX`, `kHomePose` |
| Classes | `PascalCase` | `Joint`, `MyBraccio` |
| Private members | leading `_` | `_currentAngle` (BraccioV2's style) |

Avoid names like `x`, `a1` or `temp2`. `shoulderAngle` costs nothing and saves hours of confusion later.

---

## :material-robot-industrial: Arm Lab: types on a real chip

!!! arm "Arm Lab 02"
    This sketch prints the size of each type **on the Arduino**, shows overflow, and then swings the base using
    named constants and a global counter.

```cpp title="L02_types.ino"
--8<-- "examples/arm_labs/L02_types/L02_types.ino"
```

**Compare** the sizes printed by the Arduino with the ones printed by `sizes.cpp` on your PC.

**Try this:**

1. Change `SWING` to `100`. What does the base do? (`90 + 100 = 190` is out of range.)
2. Move `int swings = 0;` inside `loop()`. What does the counter print now, and why?
3. Try `SWING = 50;` inside `loop()`. Read the compiler error.

??? success "Answers"
    1. BraccioV2 clamps 190 to 180 and −10 to 0, so the arm swings to the extremes but never past them.
    2. It prints `1` every time: a local variable is re-created with value 0 on every call to `loop()`.
    3. `assignment of read-only variable 'SWING'`. `const` protects you.

---

## Exercises

**1. Pick the type.** Choose the best type for: (a) whether the arm is powered, (b) the gripper angle,
(c) the time since start in ms, (d) an average of several angle readings, (e) the command letter typed by the user.

**2. Predict the output.** Without running it, what does this print on an **Arduino UNO**?
```cpp
int a = 300 * 200;
Serial.println(a);
```

**3. Fix the timer.** Rewrite this so it's correct for any running time:
```cpp
int start = millis();
while (millis() < start + 5000) { arm.update(); }
```

**4. Degrees ↔ microseconds.** A servo pulse is 544 µs at 0° and 2400 µs at 180°. Write a desktop program that
computes the pulse width for 0°, 45°, 90°, 135° and 180°, using integer maths only.

??? success "Solution 1"
    (a) `bool` (b) `uint8_t` or `int` (c) `unsigned long` (d) `float` (e) `char`

??? success "Solution 2"
    `300 * 200 = 60000`, which doesn't fit in a 16-bit `int` (max 32 767). The result overflows and prints
    **−5536** (60000 − 65536). On a PC with 32-bit `int` it prints 60000. Fix: `long a = 300L * 200;`

??? success "Solution 3"
    ```cpp
    unsigned long start = millis();
    while (millis() - start < 5000UL) {
      arm.update();
      delay(10);
    }
    ```

??? success "Solution 4"
    ```cpp
    #include <iostream>

    int main() {
        const long MIN_US = 544;
        const long MAX_US = 2400;
        for (int deg = 0; deg <= 180; deg += 45) {
            long us = MIN_US + (MAX_US - MIN_US) * deg / 180;   // multiply BEFORE dividing
            std::cout << deg << " deg -> " << us << " us\n";
        }
        return 0;
    }
    ```
    Output: 544, 1008, 1472, 1936, 2400. (Loops are covered properly in the next lesson.)

---

## Recap

- Every variable has a **type** that fixes its size and meaning. `int` is **16-bit** on the UNO.
- Use **fixed-width** types (`uint8_t`, `int16_t`, `uint32_t`) when size matters, and **`unsigned long`** for time.
- Overflow silently wraps. Compare durations with `millis() - start < interval`.
- Prefer **`const` / `constexpr`** over `#define`.
- Local variables die at the end of their block. Globals live forever.
- `int / int` truncates. Cast or use floating point when you need decimals.

## Further reading

- [LearnCpp: Fundamental data types](https://www.learncpp.com/cpp-tutorial/introduction-to-fundamental-data-types/)
- [LearnCpp: Fixed-width integers](https://www.learncpp.com/cpp-tutorial/fixed-width-integers-and-size-t/)
- [Arduino reference: Data types](https://docs.arduino.cc/language-reference/#variables)
- [Arduino: millis() and rollover](https://docs.arduino.cc/language-reference/en/functions/time/millis/)
