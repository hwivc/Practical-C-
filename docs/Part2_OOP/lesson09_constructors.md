# Lesson 09: Constructors

<div class="lesson-meta"><span>⏱ 75 min</span><span>🎯 Intermediate</span><span>🧩 Prerequisite: Lesson 08</span></div>

!!! abstract "What you'll learn"
    - Why objects should be **valid from the moment they're created**
    - Writing constructors, the **member initializer list**, and default arguments
    - Default constructors, overloaded constructors, and `const` members
    - Destructors and object **lifetime**
    - The Arduino rule: **no hardware in global constructors**, which is why BraccioV2 has `begin()`
    - Calibrating your arm with a `JointConfig` class

---

## The problem: half-built objects

In Lesson 08 we built joints like this:

```cpp
Joint shoulder;
shoulder.name = "shoulder";
shoulder.minAngle = 15;
// oops, forgot maxAngle... it silently stays 180
```

Every line you have to remember is a line you can forget. A **constructor** is a special method that runs automatically
when an object is created, so the object is complete and valid from its first moment:

```cpp
Joint shoulder("shoulder", 15, 165, 90);   // all rules set in one step
```

---

## Writing a constructor

A constructor has the **same name as the class** and **no return type** (not even `void`):

```cpp title="constructor_basic.cpp"
#include <iostream>

class Joint {
public:
    Joint(const char* name, int minAngle, int maxAngle, int home)   // constructor
        : _name(name), _min(minAngle), _max(maxAngle), _angle(home)  // (1)!
    {
        std::cout << "Joint '" << _name << "' created at " << _angle << '\n';
    }

    void print() {
        std::cout << _name << " [" << _min << ".." << _max << "] at " << _angle << '\n';
    }

private:
    const char* _name;
    int _min;
    int _max;
    int _angle;
};

int main() {
    Joint shoulder("shoulder", 15, 165, 90);
    Joint gripper("gripper", 10, 73, 10);
    shoulder.print();
    gripper.print();
    return 0;
}
```

1. The **member initializer list**: after the `:`, each member is initialised directly with a value. This happens
   *before* the constructor body `{ ... }` runs.

### Why use the initializer list?

You could assign inside the body (`_min = minAngle;`), but the initializer list is better:

| Initializer list `: _min(minAngle)` | Assignment in body `{ _min = minAngle; }` |
|---|---|
| Member is **created** with the right value | Member is created first, then **overwritten** |
| Works for `const` members and references | ❌ impossible for `const` and references |
| Works for members that are objects without a default constructor | ❌ compile error |

!!! warning "Order matters"
    Members are always initialised in the order they're **declared in the class**, not the order you list them after `:`.
    Keep both in the same order (`-Wall` warns if they differ), and never initialise one member from another that's
    declared later.

---

## `const` members

Some things should never change after construction, like which physical joint an object controls:

```cpp title="const_member.cpp"
#include <iostream>

class Joint {
public:
    Joint(int index, int home) : _index(index), _angle(home) {}   // const must be set here
    void moveTo(int a) { _angle = a; }
    // void rewire(int i) { _index = i; }   // would NOT compile: _index is const
    void print() { std::cout << "joint #" << _index << " at " << _angle << '\n'; }
private:
    const int _index;
    int _angle;
};

int main() {
    Joint elbow(2, 90);
    elbow.moveTo(45);
    elbow.print();   // joint #2 at 45
    return 0;
}
```

---

## Default and overloaded constructors

A **default constructor** takes no arguments. You need one whenever you write `Joint j;`, or create an **array** of objects.

```cpp title="overloaded_constructors.cpp"
#include <iostream>

class Joint {
public:
    // 1. Default constructor: a sensible "generic joint"
    Joint() : Joint("unnamed", 0, 180, 90) {}          // delegates to constructor 3

    // 2. Only a name: use the full 0..180 range
    explicit Joint(const char* name) : Joint(name, 0, 180, 90) {}

    // 3. The "real" constructor, with a default for home
    Joint(const char* name, int minA, int maxA, int home = 90)
        : _name(name), _min(minA), _max(maxA), _angle(home) {}

    void print() {
        std::cout << _name << " [" << _min << ".." << _max << "] at " << _angle << '\n';
    }

private:
    const char* _name;
    int _min, _max, _angle;
};

int main() {
    Joint a;                              // constructor 1
    Joint b("wrist");                     // constructor 2
    Joint c("gripper", 10, 73, 10);       // constructor 3
    Joint d("shoulder", 15, 165);         // constructor 3, home defaults to 90
    a.print(); b.print(); c.print(); d.print();
    return 0;
}
```

- `: Joint(...)` in constructors 1 and 2 **delegates** to another constructor, so the setup logic lives in one place.
- `explicit` stops the compiler from silently converting a `const char*` into a `Joint`, which is a good habit for
  single-argument constructors.

### Default member initialisers

You can also give members a default right where they're declared. A constructor's initializer list overrides it:

```cpp
class Joint {
    int _angle = 90;       // used unless a constructor says otherwise
    int _delta = 1;
};
```

BraccioV2 does this for its limit arrays: `int _jointMax[7] = {180, 165, 180, 180, 180, 73};`

---

## Destructors and lifetime

A **destructor** (`~ClassName()`) runs automatically when an object is destroyed. For a local object, that happens
at the end of its block:

```cpp title="lifetime.cpp"
#include <iostream>

class Tracer {
public:
    explicit Tracer(const char* n) : _n(n) { std::cout << "  + " << _n << " created\n"; }
    ~Tracer()                             { std::cout << "  - " << _n << " destroyed\n"; }
private:
    const char* _n;
};

Tracer globalArm("global arm");   // created BEFORE main() starts

int main() {
    std::cout << "main starts\n";
    Tracer a("joint A");
    {
        Tracer b("joint B (inner block)");
    }   // b destroyed here
    std::cout << "main ends\n";
    return 0;
}   // a destroyed here; globalArm after main returns
```

Output:

```text
  + global arm created
main starts
  + joint A created
  + joint B (inner block) created
  - joint B (inner block) destroyed
main ends
  - joint A destroyed
  - global arm destroyed
```

On the Arduino, objects in `setup()`/`loop()` come and go like this too, but **global** objects live forever (the
program never ends), so their destructors never run.

---

## The Arduino rule: no hardware in global constructors

Look at this line in `BraccioV2.cpp`:

```cpp
Braccio::Braccio() {
}
```

An **empty** constructor. All the real setup (attaching servos, pin 12, soft start) is in `begin()`. Why?

```mermaid
sequenceDiagram
    participant Boot as Power on
    participant G as Global constructors
    participant Init as Arduino init()
    participant S as setup()
    Boot->>G: Braccio arm; is constructed HERE
    Note over G: timers, PWM and millis()<br/>are NOT ready yet!
    G->>Init: hidden main() calls init()
    Note over Init: hardware now configured
    Init->>S: your setup() runs
    Note over S: arm.begin() is safe here
```

Global objects are constructed **before** the Arduino core's `init()` sets up the timers that `millis()`, `delay()`
and `Servo` depend on. Hardware calls in a global constructor might silently fail, or hang. So Arduino libraries follow
this pattern:

!!! tip "The `begin()` pattern"
    - **Constructor:** store configuration only (pins, limits, names). No hardware.
    - **`begin()`:** called from `setup()`. Touch the hardware here.

    You've already used it with `Serial.begin(9600)` and `arm.begin()`. Your `MyBraccio` library will do the same.

---

## :material-robot-industrial: Arm Lab: calibration objects

!!! arm "Arm Lab 09"
    Each `JointConfig` object holds one joint's name, limits and **calibrated centre**. In `setup()` we apply them to
    the arm *before* `arm.begin()`, so the arm stands up at your calibrated "straight".
    Replace the numbers with the ones from your Lesson 00 notebook.

```cpp title="L09_calibration.ino"
--8<-- "examples/arm_labs/L09_calibration/L09_calibration.ino"
```

!!! note "Two things you haven't met yet"
    - `Braccio& arm` in `applyTo`: the `&` means "use the real arm, not a copy". See [Lesson 13](../Part3_Memory/lesson13_references.md).
    - `const` after a method name, as in `void print() const`: "this method doesn't change the object". See [Lesson 11](lesson11_methods.md).

**Try this:**

1. Find the value that makes your shoulder truly vertical, and put it in `shoulderCfg`.
2. Add a `JointConfig` constructor overload that takes only `(name, index)` and uses 0–180 with centre 90.
3. Move `arm.begin()` *above* the `applyTo` calls. What changes in the start-up pose?

??? success "Answer to 3"
    `begin()` moves to the centres that are set *at that moment*: the library defaults (90, gripper 50). Your
    calibration then only affects later moves, so the start-up pose is uncalibrated. **Order matters.**

---

## Common mistakes

| Mistake | Symptom |
|---|---|
| Giving a constructor a return type (`void Joint(...)`) | Compile error: it's parsed as a normal method |
| `Joint j();` to call the default constructor | Declares a **function** named `j` (the "most vexing parse"). Write `Joint j;` or `Joint j{};` |
| Not initialising a `const` member in the list | `uninitialized const member` |
| Initializer order differs from declaration order | `-Wreorder` warning; possible use of an uninitialised value |
| Calling `Serial`, `Servo::attach` or `delay` in a global object's constructor | Works sometimes, fails mysteriously other times. Use `begin()`. |

---

## Exercises

**1. `Servo`-like class.** Write `class FakeServo` with a constructor `FakeServo(int pin)` storing a `const int _pin`, an
`int _angle` initialised to 90, and methods `write(int)` and `print()`.

**2. Constructor validation.** Write a `Joint` constructor that fixes bad input: if `minA > maxA`, swap them; if `home` is
outside the range, clamp it. Test with `Joint j("bad", 170, 20, 5);`.

**3. Count objects.** Add a global `int jointsCreated` that the constructor increments. Create five joints and print the count.

??? success "Solution 2"
    ```cpp
    #include <iostream>

    class Joint {
    public:
        Joint(const char* name, int minA, int maxA, int home)
            : _name(name), _min(minA < maxA ? minA : maxA), _max(minA < maxA ? maxA : minA), _angle(home) {
            if (_angle < _min) _angle = _min;
            if (_angle > _max) _angle = _max;
        }
        void print() { std::cout << _name << " [" << _min << ".." << _max << "] at " << _angle << '\n'; }
    private:
        const char* _name;
        int _min, _max, _angle;
    };

    int main() {
        Joint j("bad", 170, 20, 5);
        j.print();   // bad [20..170] at 20
        return 0;
    }
    ```

??? success "Solution 3"
    ```cpp
    #include <iostream>

    int jointsCreated = 0;

    class Joint {
    public:
        explicit Joint(int home) : _angle(home) { jointsCreated++; }
    private:
        int _angle;
    };

    int main() {
        Joint a(90), b(90), c(45), d(180), e(10);
        std::cout << jointsCreated << " joints created\n";   // 5
        return 0;
    }
    ```
    In [Lesson 11](lesson11_methods.md) you'll replace the global with a `static` class member.

---

## Recap

- A constructor has the class's name and no return type, and runs automatically at creation.
- Initialise members in the **initializer list**. It's required for `const` members and references.
- Use overloads, default arguments and delegation to offer convenient ways to build an object.
- Destructors run when an object's lifetime ends. Global Arduino objects never die.
- **Constructors store configuration. `begin()` touches hardware.**

## Further reading

- [LearnCpp: Constructors](https://www.learncpp.com/cpp-tutorial/introduction-to-constructors/)
- [LearnCpp: Member initializer lists](https://www.learncpp.com/cpp-tutorial/constructor-member-initializer-lists/)
- [Arduino style guide for libraries (the `begin()` convention)](https://docs.arduino.cc/learn/contributions/arduino-library-style-guide/)
