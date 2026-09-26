# Lesson 12: Pointers

<div class="lesson-meta"><span>⏱ 90 min</span><span>🎯 Intermediate</span><span>🧩 Prerequisite: Part 2</span></div>

!!! abstract "What you'll learn"
    - Memory as a row of numbered boxes: **addresses**
    - The address-of operator `&` and the dereference operator `*`
    - `nullptr` and why you must check before dereferencing
    - Pointers to structs and objects, and the arrow operator `->`
    - Passing pointers to functions so they can modify the caller's data
    - `const` with pointers
    - A keyboard **jog controller** that "points at" the selected joint

---

## Memory is a row of numbered boxes

Every variable lives somewhere in RAM, and every byte of RAM has a number called its **address**. On the Arduino UNO, SRAM
addresses run from `0x0100` to `0x08FF` (that's 2 048 bytes).

<figure markdown>
![Pointers and memory](../images/pointer_memory.svg){ .diagram }
<figcaption>A pointer is simply a variable whose value is an address. <code>ptr</code> "points at" <code>angle</code>.</figcaption>
</figure>

| Syntax | Read it as | Example |
|---|---|---|
| `int* p;` | "p is a pointer to an int" | declares a pointer |
| `&x` | "the address of x" | `p = &angle;` |
| `*p` | "the value p points at" | `*p = 120;` |
| `p->member` | "member of the object p points at" | `j->angle` (same as `(*p).angle`) |
| `nullptr` | "points at nothing" | `int* p = nullptr;` |

```cpp title="pointer_basics.cpp"
#include <iostream>

int main() {
    int angle = 90;
    int* ptr = &angle;                       // ptr holds the ADDRESS of angle

    std::cout << "angle      = " << angle << '\n';
    std::cout << "&angle     = " << &angle << '\n';     // some address like 0x7ffd...
    std::cout << "ptr        = " << ptr << '\n';        // the same address
    std::cout << "*ptr       = " << *ptr << '\n';       // 90: the value at that address

    *ptr = 120;                              // write THROUGH the pointer
    std::cout << "angle now  = " << angle << '\n';      // 120

    int maxAngle = 165;
    ptr = &maxAngle;                         // a pointer can be re-aimed
    *ptr -= 5;
    std::cout << "maxAngle   = " << maxAngle << '\n';   // 160
    std::cout << "sizeof(ptr) = " << sizeof(ptr) << " bytes on this PC\n";   // 8 on 64-bit PCs, 2 on the UNO
    return 0;
}
```

!!! note "Two meanings of `*` and `&`"
    In a **declaration**, `int* p` means "p is a pointer" and `int& r` means "r is a reference" (next lesson).
    In an **expression**, `*p` means "dereference" and `&x` means "address of". Same symbols, different jobs.

---

## Why would a robot need pointers?

1. **To modify the caller's variables** from inside a function (pass-by-value gives the function only a copy).
2. **To choose between objects at run time.** "The currently selected joint" can point at any of the six.
3. **To avoid copying** large objects when calling functions.
4. **Hardware registers** live at fixed addresses. The Arduino core itself uses pointers to reach pins and timers.
5. **Arrays** are closely tied to pointers ([Lesson 15](lesson15_arrays.md)).

---

## Pointers as function parameters

```cpp title="pointer_params.cpp"
#include <iostream>

// Pass-by-value: gets a copy, can't change the caller's variable
void clampCopy(int angle, int lo, int hi) {
    if (angle < lo) angle = lo;
    if (angle > hi) angle = hi;
}

// Pass-by-pointer: gets the address, CAN change the caller's variable
void clampInPlace(int* angle, int lo, int hi) {
    if (angle == nullptr) return;          // defensive check
    if (*angle < lo) *angle = lo;
    if (*angle > hi) *angle = hi;
}

int main() {
    int shoulder = 200;
    clampCopy(shoulder, 15, 165);
    std::cout << "after clampCopy:    " << shoulder << '\n';   // 200, unchanged
    clampInPlace(&shoulder, 15, 165);
    std::cout << "after clampInPlace: " << shoulder << '\n';   // 165
    return 0;
}
```

In the next lesson you'll see that **references** do this more safely. Pointers remain useful when "nothing" (`nullptr`) is
a valid option, or when you need to re-aim at different objects.

---

## Pointers to structs and objects: `->`

```cpp title="arrow.cpp"
#include <iostream>

struct JointState {
    const char* name;
    int angle;
};

void nudge(JointState* j, int degrees) {
    j->angle += degrees;             // same as (*j).angle += degrees
}

int main() {
    JointState base  = {"base", 90};
    JointState elbow = {"elbow", 45};

    JointState* selected = &base;    // choose a joint at run time
    nudge(selected, 10);
    selected = &elbow;               // switch selection
    nudge(selected, -5);

    std::cout << base.name << " " << base.angle << ", " << elbow.name << " " << elbow.angle << '\n';   // base 100, elbow 40
    return 0;
}
```

---

## `nullptr`: pointing at nothing

A pointer that doesn't point at anything valid should be `nullptr`. **Dereferencing `nullptr` (or garbage) is undefined
behaviour.** On a PC the program crashes. On the Arduino there's no crash message: it may reset, freeze, or quietly
corrupt other variables. On a robot, that could mean a joint moving somewhere random.

```cpp
JointState* selected = nullptr;   // nothing selected yet
// ...
if (selected != nullptr) {        // ALWAYS check before using
    selected->angle = 90;
}
```

!!! danger "Dangling pointers"
    A pointer to a local variable becomes invalid ("dangling") when that variable's function returns:

    ```cpp
    int* badIdea() {
        int angle = 90;
        return &angle;    // angle is destroyed when the function returns!
    }
    ```

    `-Wall` warns: *address of local variable returned*. Never keep a pointer to something that may die before the pointer does.

---

## `const` and pointers

| Declaration | Can change the value pointed at? | Can re-aim the pointer? |
|---|:-:|:-:|
| `int* p` | ✅ | ✅ |
| `const int* p` (pointer to const) | ❌ | ✅ |
| `int* const p` (const pointer) | ✅ | ❌ |
| `const int* const p` | ❌ | ❌ |

Read declarations **right to left**: `const int* p` is "p is a pointer to an int that is const". Use `const T*` for
function parameters that only need to *read*, like `show(const JointState* j)` in the Arm Lab.

---

## `this` is a pointer

Inside any method, `this` is a pointer to the object the method was called on. `this->_angle` and `_angle` mean the same
thing there. You've been using `this` implicitly since Lesson 08.

---

## :material-robot-industrial: Arm Lab: jog controller

!!! arm "Arm Lab 12"
    Open the Serial Monitor (line ending: *No line ending* works best). Press `1`–`6` to select a joint, then `+` / `-`
    to move it in 5° steps. `?` prints every joint, with `>` marking the selected one.
    `selected` is a pointer that gets re-aimed at a different `JointState` each time you press a number.

```cpp title="L12_jog_pointer.ino"
--8<-- "examples/arm_labs/L12_jog_pointer/L12_jog_pointer.ino"
```

!!! tip "This is how you calibrate"
    Use the jog controller to find each joint's true centre and safe limits. Write the numbers into the `JointConfig`
    objects from Lesson 09. It's also the basis of the **teach & replay** project ([P4](../Projects/project04_record_replay.md)).

**Try this:**

1. Add `[` and `]` keys that jog by 1° for fine adjustment.
2. Add a `0` key that sets `selected = nullptr` ("nothing selected"), and make sure `+` / `-` then do nothing.
3. Print the **address** of the selected joint in hex: `Serial.println((unsigned int)selected, HEX);`. How far apart are
   `base` and `shoulder` in memory? Does that match `sizeof(JointState)`?

??? success "Answer to 3"
    On the UNO, `sizeof(JointState)` is 10 bytes (a 2-byte pointer + four 2-byte ints), and consecutive globals are usually
    placed 10 bytes apart. The exact layout is up to the compiler, but printing addresses is a great way to see
    that RAM really is just numbered boxes.

---

## Common mistakes

| Mistake | Result |
|---|---|
| Using an uninitialised pointer: `int* p; *p = 5;` | Writes to a random address, which can crash or corrupt memory |
| Forgetting `*`: `p = 120;` instead of `*p = 120;` | Compile error (can't assign an int to a pointer), luckily |
| Forgetting `&`: `int* p = angle;` | Compile error: invalid conversion from `int` to `int*` |
| Dereferencing `nullptr` | Undefined behaviour (crash on PC, silent chaos on Arduino) |
| Returning the address of a local variable | Dangling pointer |

---

## Exercises

**1. Swap.** Write `void swapAngles(int* a, int* b)` that swaps two ints. Test it.

**2. Min/max finder.** Write `void findRange(int a, int b, int c, int* outMin, int* outMax)` that writes the smallest and
largest of three angles through the two output pointers. This is how C-style functions "return" several values.

**3. Trace it.** What does this print?
```cpp
int x = 10, y = 20;
int* p = &x;
int* q = &y;
*p = *q + 5;
p = q;
*p = 99;
std::cout << x << ' ' << y << '\n';
```

??? success "Solution 1 & 2"
    ```cpp
    #include <iostream>

    void swapAngles(int* a, int* b) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }

    void findRange(int a, int b, int c, int* outMin, int* outMax) {
        int lo = a, hi = a;
        if (b < lo) lo = b;
        if (c < lo) lo = c;
        if (b > hi) hi = b;
        if (c > hi) hi = c;
        if (outMin) *outMin = lo;    // outputs are optional: nullptr means "don't care"
        if (outMax) *outMax = hi;
    }

    int main() {
        int s = 30, e = 150;
        swapAngles(&s, &e);
        std::cout << s << ' ' << e << '\n';       // 150 30

        int lo, hi;
        findRange(90, 15, 165, &lo, &hi);
        std::cout << lo << ".." << hi << '\n';    // 15..165
        return 0;
    }
    ```

??? success "Solution 3"
    `25 99`. First `*p = *q + 5` sets `x` to 25. Then `p = q` re-aims `p` at `y`, so `*p = 99` changes `y`.

---

## Recap

- Every variable has an **address**. A pointer stores an address.
- `&x` gets an address, `*p` follows it, `p->m` reaches a member through it.
- Initialise pointers, prefer `nullptr` for "nothing", and **check before dereferencing**.
- Pointers let functions modify callers' data and let code choose objects at run time.
- `const T*` = read-only access through the pointer.

## Further reading

- [LearnCpp: Introduction to pointers](https://www.learncpp.com/cpp-tutorial/introduction-to-pointers/)
- [LearnCpp: Null pointers](https://www.learncpp.com/cpp-tutorial/null-pointers/)
- [Arduino: The pointer access operators](https://docs.arduino.cc/language-reference/en/structure/pointer-access-operators/dereference/)
