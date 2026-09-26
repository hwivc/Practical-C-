# Lesson 17: Anatomy of a Library

<div class="lesson-meta"><span>⏱ 60 min</span><span>🎯 Intermediate</span><span>🧩 Prerequisite: Lesson 16</span></div>

!!! abstract "What you'll learn"
    - The folder layout of an Arduino library (the "1.5 format")
    - `library.properties`, `keywords.txt`, `examples/`, `README` and `LICENSE`
    - Designing a library's **public API** in its header
    - **Namespaces** to avoid name clashes
    - Installing your own library locally, sharing it as a ZIP, and publishing it
    - Open-source licences: what BraccioV2's LGPL/GPL means for you
    - Building **ArmPoses**, a real library you can install

---

## A library is a folder with rules

In Lesson 05 you split a sketch into tabs. A **library** takes those files out of the sketch folder so that *any* sketch
can use them. Here's the standard layout, using the ArmPoses library you'll install in this lesson:

```text
ArmPoses/                       ← folder name = library name
├── library.properties          ← metadata: name, version, author, dependencies
├── keywords.txt                ← syntax colouring in the Arduino IDE
├── README.md                   ← what it does, how to use it
├── LICENSE                     ← the legal terms for reuse
├── src/                        ← ALL source code goes here
│   ├── ArmPoses.h              ←   public header (what users #include)
│   └── ArmPoses.cpp            ←   implementation
└── examples/                   ← shows up under File → Examples → ArmPoses
    └── PoseDance/
        └── PoseDance.ino       ←   folder and .ino must have the same name
```

BraccioV2's [repository](https://github.com/kk6axq/BraccioV2) has the same shape: `src/BraccioV2.h`, `src/BraccioV2.cpp`,
`examples/Basic_Movement`, `examples/Advanced_Movement`, `examples/Braccio_Calibration` and a `library.properties`.

---

## `library.properties`

```ini title="ArmPoses/library.properties"
--8<-- "examples/libraries/ArmPoses/library.properties"
```

| Field | Purpose |
|---|---|
| `name` | unique name shown in the Library Manager |
| `version` | [semantic version](https://semver.org): **MAJOR.MINOR.PATCH** |
| `sentence` / `paragraph` | short and long description |
| `category` | one of the fixed categories (`Device Control`, `Sensors`, `Communication`, …) |
| `architectures` | which boards it supports (`avr`, `samd`, `*` for all) |
| `depends` | other libraries the Library Manager should install too |

Compare with BraccioV2's actual file:

```ini
name=BraccioV2
version=0.2.1
author=Lukas Severinghaus <kk6axq@gmail.com>
sentence=A library that enables more functionality for use with the Tinkerkit Braccio Arm.
category=Device Control
url=https://github.com/kk6axq/BraccioV2
architectures=avr, samd, sam
depends=Servo
```

!!! info "Semantic versioning in one sentence"
    Bump **PATCH** (1.0.0 → 1.0.1) for bug fixes, **MINOR** (→ 1.1.0) for new features that don't break existing sketches,
    and **MAJOR** (→ 2.0.0) when existing sketches would need changes. BraccioV2 is still `0.x`, which by convention means
    "the API may still change".

## `keywords.txt`

Tells the IDE which words to colour. Each line is `word<TAB>KEYWORDn`, and the separator **must be a tab**:

```text
Pose        KEYWORD1      ← classes and types
PosePlayer  KEYWORD1
moveTo      KEYWORD2      ← methods and functions
play        KEYWORD2
HOME        LITERAL1      ← constants
```

---

## Designing the public header

The header is your library's **contract**: users read it to learn what they can do. Aim for:

- a small, clear set of public operations,
- everything else `private` (Lesson 10),
- include guards (Lesson 06),
- no `#define` for things users might also name (Lesson 07),
- comments on every public method.

```cpp title="ArmPoses/src/ArmPoses.h"
--8<-- "examples/libraries/ArmPoses/src/ArmPoses.h"
```

```cpp title="ArmPoses/src/ArmPoses.cpp"
--8<-- "examples/libraries/ArmPoses/src/ArmPoses.cpp"
```

Things to notice:

- `PosePlayer` holds a **reference** to the arm (Lesson 13), so it controls your `arm` object and never a copy.
- `moveTo` takes `const Pose&`: no copy, read-only.
- `current()` returns a `const Pose&`: the caller can read the player's state but not change it.
- `_lerp` is `private static`: a helper that needs no object data (Lesson 11).

### Namespaces

Two libraries could both define `HOME`. A **namespace** puts names in their own "surname":

```cpp title="namespaces.cpp"
#include <iostream>

namespace Poses {
    const int HOME_BASE = 90;
}
namespace Calibration {
    const int HOME_BASE = 93;          // same name, different namespace: no clash
}

int main() {
    std::cout << Poses::HOME_BASE << ' ' << Calibration::HOME_BASE << '\n';   // 90 93
    using namespace Poses;             // "search Poses too" (use sparingly, never in headers)
    std::cout << HOME_BASE << '\n';    // 90
    return 0;
}
```

You've used a namespace since Lesson 01: `std::cout` is `cout` from the `std` namespace. Macros (`#define`) **ignore**
namespaces, which is another reason to avoid them in libraries.

---

## Using the library

```cpp title="ArmPoses/examples/PoseDance/PoseDance.ino"
--8<-- "examples/libraries/ArmPoses/examples/PoseDance/PoseDance.ino"
```

---

## :material-robot-industrial: Arm Lab: install your first library

!!! arm "Arm Lab 17"
    1. Find your **sketchbook** folder: *File → Preferences → Sketchbook location* (e.g. `Documents/Arduino`).
    2. Copy the course's `examples/libraries/ArmPoses` folder into `Documents/Arduino/libraries/`.
    3. **Restart** the Arduino IDE.
    4. Open **File → Examples → ArmPoses → PoseDance** and upload it.
    5. Open a *new* sketch and type `#include <ArmPoses.h>`. Notice `Pose` and `PosePlayer` are coloured (that's `keywords.txt`).

    Any sketch on your computer can now use `PosePlayer`.

!!! tip "Sharing it"
    - **ZIP:** compress the `ArmPoses` folder. Others install it with *Sketch → Include Library → Add .ZIP Library…*
    - **Library Manager:** put it in a public GitHub repo, create a release tag (e.g. `1.0.0`), and submit it via the
      [Arduino Library Registry](https://github.com/arduino/library-registry). The registry checks your layout with
      [arduino-lint](https://arduino.github.io/arduino-lint/).

**Try this:**

1. Add a `const Pose WAVE_UP` and `WAVE_DOWN` to `namespace Poses` and a new example `Wave`.
2. Bump the version to `1.1.0` in `library.properties`. Is that the right kind of bump? Why?
3. Rename `moveTo` to `glideTo`. Which version number would that need?

??? success "Answers"
    2. Yes, **MINOR**: new features were added, and existing sketches still compile.
    3. **MAJOR** (2.0.0): existing sketches calling `moveTo` would break.

---

## Licences: can I copy BraccioV2?

Open-source code is not "free of rules". The licence says what you may do:

| Licence | You may use it in your project | If you distribute a modified version of the library itself… |
|---|---|---|
| **MIT / BSD** | ✅ | …keep the copyright notice |
| **LGPL 2.1** (BraccioV2's file headers) | ✅ even in closed-source projects | …you must share your changes to the library under LGPL |
| **GPL 3** (BraccioV2's repository) | ✅ | …your whole program must also be GPL if you distribute it |

BraccioV2's source files carry an **LGPL 2.1** notice, while its repository states **GPL V3**. In this course we *study*
it and write our own `MyBraccio` from scratch, crediting the original design. When you publish your own library, add a
`LICENSE` file ([choosealicense.com](https://choosealicense.com) helps you pick one) and credit whatever inspired it.

*(This is general information, not legal advice.)*

---

## Exercises

**1. Spot the problems.** What's wrong with this library layout?
```text
Robot Helpers/
├── RobotHelpers.h
├── RobotHelpers.cpp
└── examples/
    └── demo.ino
```

**2. Write the properties.** Write a `library.properties` for a library `GripperSense` by you, version 0.1.0, for AVR
boards, which depends on `BraccioV2`.

**3. Namespace it.** Move `PosePlayer` into `namespace Braccio_Tools`. What changes in `PoseDance.ino`?

??? success "Solution 1"
    - The folder name has a **space**, and it doesn't match the header name. Use `RobotHelpers/`.
    - No `library.properties`, so the IDE treats it as an old-format library with limited features.
    - Sources aren't in `src/`. That's allowed in the old format, but `src/` is recommended.
    - `examples/demo.ino` must live in its own folder with the same name: `examples/demo/demo.ino`.

??? success "Solution 3"
    Every use needs the prefix: `Braccio_Tools::PosePlayer player(arm);`, or add
    `using Braccio_Tools::PosePlayer;` near the top of the sketch.

---

## Recap

- A library = a folder with `library.properties`, `src/`, `examples/`, `keywords.txt`, a README and a LICENSE.
- The header is the public contract. Keep it small, documented and guarded.
- Namespaces prevent clashes (macros don't respect them).
- Install locally by copying into `sketchbook/libraries`, share as a ZIP, or publish via the Library Registry.
- Respect licences and credit original authors.

## Further reading

- [Arduino Library specification](https://arduino.github.io/arduino-cli/latest/library-specification/)
- [Arduino: Writing a library](https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide/)
- [Arduino library style guide](https://docs.arduino.cc/learn/contributions/arduino-library-style-guide/)
- [Arduino Library Registry](https://github.com/arduino/library-registry)
- [choosealicense.com](https://choosealicense.com/)
