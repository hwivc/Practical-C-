# C++ & Arduino Cheatsheet

Everything from the course on one page. Print it and keep it next to the arm.

---

## Program structure

=== "Desktop C++"

    ```cpp
    #include <iostream>

    int main() {
        std::cout << "Hello\n";
        return 0;
    }
    ```
    `g++ -std=c++17 -Wall -Wextra main.cpp -o main`

=== "Arduino sketch"

    ```cpp
    #include <MyBraccio.h>
    MyBraccio arm;

    void setup() {            // once
        Serial.begin(9600);
        arm.begin();
    }

    void loop() {             // forever
        arm.update();
    }
    ```

## Types (Arduino UNO sizes)

| Type | Bytes | Range / use |
|---|:-:|---|
| `bool` | 1 | `true` / `false` |
| `char` | 1 | `'A'`, −128…127 |
| `uint8_t` / `byte` | 1 | 0…255 (angles, pins) |
| `int` / `int16_t` | **2** | −32 768…32 767 |
| `unsigned int` / `uint16_t` | 2 | 0…65 535 |
| `long` / `int32_t` | 4 | ±2.1 billion |
| `unsigned long` / `uint32_t` | 4 | **`millis()`**, `micros()` |
| `float` / `double` | 4 | ~7 digits (both are 32-bit on the UNO) |

```cpp
const int MAX_ANGLE = 180;          // typed constant
constexpr int NUM_JOINTS = 6;       // compile-time constant
int x = 7 / 2;                      // 3 (integer division)
double y = 7.0 / 2;                 // 3.5
int z = static_cast<int>(3.7);      // 3
```

## Operators

| | |
|---|---|
| Arithmetic | `+ - * / %` · `+= -= *= /=` · `++ --` |
| Compare | `== != < > <= >=` |
| Logic | <code>&amp;&amp; &#124;&#124; !</code> |
| Conditional | `cond ? a : b` |
| Address / pointer | `&x` address · `*p` value · `p->m` member |

## Control flow

```cpp
if (a < 15) { ... } else if (a > 165) { ... } else { ... }

switch (cmd) {
  case 'O': open(); break;
  case 'C': close(); break;
  default: break;
}

for (int j = 0; j < 6; j++) { ... }
for (int& a : angles) { ... }       // range-for, by reference
while (arm.isMoving()) { arm.update(); }
do { ... } while (cond);
```

## Functions

```cpp
int clampAngle(int a, int lo, int hi);            // declaration (prototype)
int clampAngle(int a, int lo, int hi) {           // definition
    return a < lo ? lo : (a > hi ? hi : a);
}
void moveTo(const Pose& p, unsigned long ms = 1500);   // const ref + default arg
void swapAngles(int& a, int& b);                        // modify caller's variables
```

## Classes

```cpp
class Joint {
public:
    Joint(int lo, int hi) : _min(lo), _max(hi), _angle(lo) {}   // initializer list
    bool setAngle(int a);                      // declared here...
    int angle() const { return _angle; }       // const = read-only method
    static int count();                        // belongs to the class
private:
    int _min, _max, _angle;
};

bool Joint::setAngle(int a) { ... }            // ...defined outside with ::
```

| Keyword | Meaning |
|---|---|
| `public` / `private` | who can access |
| `const` after a method | doesn't modify the object |
| `static` member | one copy shared by all objects |
| `explicit` | no implicit conversion via this constructor |
| `enum class State { A, B };` | scoped, type-safe names |
| `namespace kin { ... }` | groups names: `kin::L1` |

## Pointers, references, arrays

```cpp
int a = 90;
int* p = &a;     *p = 120;          // pointer
int& r = a;      r = 45;            // reference (alias)
int* none = nullptr;                // points at nothing: check before use!

int pose[6] = {90, 45, 180, 180, 90, 10};
int n = sizeof(pose) / sizeof(pose[0]);          // 6
int seq[][6] = {{...}, {...}};                   // 2-D
void f(const int arr[], int count);              // arrays decay: pass the length

char buf[32];                                    // C string: ends with '\0'
strcmp(a, b) == 0                                // equal
strtol(s, &end, 10)                              // parse an int safely
```

## Headers

```cpp
#ifndef MY_FILE_H        // include guard
#define MY_FILE_H
// declarations, class definitions, const values, extern variables
#endif
```

`#include <Lib.h>` = libraries · `#include "file.h"` = your files · `.h` declares, `.cpp` defines · define everything exactly **once**.

## Preprocessor

```cpp
#define DEBUG
#ifdef DEBUG
  #define LOG(x) Serial.println(x)
#else
  #define LOG(x)
#endif
#if defined(ARDUINO_ARCH_AVR) ... #endif
__FILE__ __LINE__ __DATE__ __TIME__
```

## Arduino essentials

| Function | Use |
|---|---|
| `pinMode(p, OUTPUT/INPUT/INPUT_PULLUP)` | configure a pin |
| `digitalWrite(p, HIGH)` / `digitalRead(p)` | digital I/O |
| `analogRead(A0)` | 0…1023 |
| `map(v, 0, 1023, 0, 180)` | rescale |
| `constrain(v, lo, hi)` | clamp (a macro: don't pass function calls) |
| `millis()` / `micros()` | time since start (`unsigned long`) |
| `delay(ms)` | **blocks**: avoid in motion code |
| `Serial.begin(9600)`, `Serial.print(x)`, `Serial.println(F("text"))` | serial output (F() keeps text in flash) |
| `Serial.available()`, `Serial.read()` | serial input, one character at a time |
| `EEPROM.put(addr, v)` / `EEPROM.get(addr, v)` | save/load across power-off |

### Non-blocking timing

```cpp
unsigned long last = 0;
void loop() {
  if (millis() - last >= 500) {     // correct even when millis() rolls over
    last = millis();
    // every 500 ms
  }
}
```

## Braccio quick facts

| M1 base | M2 shoulder | M3 elbow | M4 wrist | M5 wrist rot | M6 gripper |
|:-:|:-:|:-:|:-:|:-:|:-:|
| pin 11 | pin 10 | pin 9 | pin 6 | pin 5 | pin 3 |
| 0–180 | 15–165 | 0–180 | 0–180 | 0–180 | 10–73 |

Soft-start: pin 12 · power: 5 V, 4–5 A · home: 90 90 90 90 90 50 · park: 90 45 180 180 90 10

## Compiler error decoder

| Message | Usually means |
|---|---|
| `expected ';' before ...` | missing `;` on the **previous** line |
| `'x' was not declared in this scope` | typo, missing `#include`, or used before declared |
| `undefined reference to ...` | **linker**: definition missing or `.cpp` not compiled |
| `multiple definition of ...` | defined in a header included twice / in two files |
| `redefinition of 'struct X'` | header without include guard |
| `'x' is private within this context` | accessing a private member from outside |
| `passing 'const X' as 'this' argument discards qualifiers` | calling a non-`const` method on a `const` object |
| `expected unqualified-id before numeric constant` | a **macro** replaced your name (e.g. `ELBOW`) |
