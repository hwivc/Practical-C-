# Glossary

Terms used in the course, with the lesson where each one is explained.

| Term | Meaning | Lesson |
|---|---|:-:|
| **Actuator** | A device that moves something. The Braccio's actuators are servo motors. | [HW](../hardware/meet_the_braccio.md) |
| **Address** | The number that identifies a byte of memory. | [12](../Part3_Memory/lesson12_pointers.md) |
| **Argument** | The actual value passed to a function when calling it. | [04](../Part1_Cpp_Basics/lesson04_functions.md) |
| **Array** | A fixed number of same-typed values stored side by side, accessed with an index starting at 0. | [15](../Part3_Memory/lesson15_arrays.md) |
| **Blocking code** | Code that makes the program wait (e.g. `delay()`), so nothing else can happen meanwhile. | [03](../Part1_Cpp_Basics/lesson03_control_flow.md) |
| **Bootloader** | A small program in the UNO's flash that receives new sketches over USB. | [16](../Part4_Arduino_Libraries/lesson16_arduino_ecosystem.md) |
| **Calibration** | Measuring and correcting each joint's true centre and limits. | [09](../Part2_OOP/lesson09_constructors.md) |
| **Clamp** | Force a value into a range: below min → min, above max → max. | [03](../Part1_Cpp_Basics/lesson03_control_flow.md) |
| **Class** | A user-defined type bundling data (members) and behaviour (methods). | [08](../Part2_OOP/lesson08_classes.md) |
| **Compiler** | Translates C++ source into machine code. | [01](../Part1_Cpp_Basics/lesson01_compiler.md) |
| **Composition** | Building an object out of other objects ("an arm *has* six servos"). | [08](../Part2_OOP/lesson08_classes.md) |
| **`const`** | Read-only. On a method: the method doesn't modify the object. | [02](../Part1_Cpp_Basics/lesson02_variables.md), [11](../Part2_OOP/lesson11_methods.md) |
| **Constructor** | Special method that initialises a new object. | [09](../Part2_OOP/lesson09_constructors.md) |
| **Core (Arduino)** | The code behind `Arduino.h`: `pinMode`, `millis`, `Serial`, the hidden `main()`. | [16](../Part4_Arduino_Libraries/lesson16_arduino_ecosystem.md) |
| **Cross-compiler** | A compiler that runs on one machine (your PC) and produces code for another (the AVR chip). | [01](../Part1_Cpp_Basics/lesson01_compiler.md) |
| **Declaration** | Tells the compiler a name exists and its type (e.g. a function prototype). | [04](../Part1_Cpp_Basics/lesson04_functions.md) |
| **Definition** | Provides the actual body or storage. Exactly one per program. | [05](../Part1_Cpp_Basics/lesson05_headers.md) |
| **Degrees of freedom (DOF)** | Independent ways a robot can move. The Braccio has 5 + a gripper. | [HW](../hardware/meet_the_braccio.md) |
| **Dereference** | Follow a pointer to the value it points at: `*p`. | [12](../Part3_Memory/lesson12_pointers.md) |
| **Destructor** | Special method that runs when an object's lifetime ends: `~Joint()`. | [09](../Part2_OOP/lesson09_constructors.md) |
| **Duty cycle** | Fraction of time a signal is HIGH. | [18](../Part4_Arduino_Libraries/lesson18_bracciov2_deep_dive.md) |
| **EEPROM** | Small memory (1 KB) that keeps data without power. | [14](../Part3_Memory/lesson14_dynamic_memory.md), [P4](../Projects/project04_record_replay.md) |
| **Encapsulation** | Hiding data behind methods that keep it valid. | [10](../Part2_OOP/lesson10_encapsulation.md) |
| **`enum class`** | A scoped, type-safe set of named constants. | [07](../Part1_Cpp_Basics/lesson07_preprocessor.md) |
| **Fake / mock** | A stand-in for hardware used in tests on the PC. | [20](../Part4_Arduino_Libraries/lesson20_deployment.md) |
| **Finite-state machine (FSM)** | A program organised as named states with transitions between them. | [P2](../Projects/project02_pick_and_place.md) |
| **Flash** | The UNO's 32 KB of program memory. | [14](../Part3_Memory/lesson14_dynamic_memory.md) |
| **Forward kinematics** | Joint angles → gripper position. | [P1](../Projects/project01_robot.md) |
| **Header file** | A `.h` file of declarations that other files `#include`. | [05](../Part1_Cpp_Basics/lesson05_headers.md) |
| **Heap** | Memory allocated at run time with `new`/`delete`. Avoided on small MCUs. | [14](../Part3_Memory/lesson14_dynamic_memory.md) |
| **Include guard** | `#ifndef`/`#define`/`#endif` that stops a header being pasted twice. | [06](../Part1_Cpp_Basics/lesson06_include_guards.md) |
| **Initializer list** | `: member(value)` after a constructor's parameters. | [09](../Part2_OOP/lesson09_constructors.md) |
| **Interpolation (lerp)** | Computing in-between values: `a + (b − a) · t`. | [13](../Part3_Memory/lesson13_references.md) |
| **Invariant** | A rule about an object that must always be true (e.g. min ≤ angle ≤ max). | [10](../Part2_OOP/lesson10_encapsulation.md) |
| **Inverse kinematics (IK)** | Desired gripper position → joint angles. | [P5](../Projects/project05_kinematics.md) |
| **Library** | Reusable code packaged in a folder with `src/`, `examples/` and `library.properties`. | [17](../Part4_Arduino_Libraries/lesson17_library_structure.md) |
| **Linker** | Joins compiled object files and libraries into one program. | [01](../Part1_Cpp_Basics/lesson01_compiler.md), [05](../Part1_Cpp_Basics/lesson05_headers.md) |
| **Macro** | A `#define` text replacement performed by the preprocessor. | [07](../Part1_Cpp_Basics/lesson07_preprocessor.md) |
| **Method** | A function that belongs to a class. | [08](../Part2_OOP/lesson08_classes.md) |
| **Namespace** | A named scope that prevents name clashes: `kin::L1`, `std::cout`. | [17](../Part4_Arduino_Libraries/lesson17_library_structure.md) |
| **Non-blocking code** | Code that does a small step and returns, letting other tasks run. | [19](../Part4_Arduino_Libraries/lesson19_coding_mybraccio.md) |
| **`nullptr`** | A pointer value meaning "points at nothing". | [12](../Part3_Memory/lesson12_pointers.md) |
| **Object** | An instance of a class. | [08](../Part2_OOP/lesson08_classes.md) |
| **One Definition Rule (ODR)** | Everything may be declared many times but defined only once. | [05](../Part1_Cpp_Basics/lesson05_headers.md) |
| **Overflow** | A value too big for its type, which wraps around or misbehaves. | [02](../Part1_Cpp_Basics/lesson02_variables.md) |
| **Overloading** | Several functions/methods with the same name but different parameters. | [04](../Part1_Cpp_Basics/lesson04_functions.md) |
| **Parameter** | A variable in a function's definition that receives an argument. | [04](../Part1_Cpp_Basics/lesson04_functions.md) |
| **Pointer** | A variable that stores an address. | [12](../Part3_Memory/lesson12_pointers.md) |
| **Pose** | A set of angles for all joints: one "position" of the whole arm. | [06](../Part1_Cpp_Basics/lesson06_include_guards.md) |
| **Preprocessor** | Text-editing stage before compilation (`#include`, `#define`, `#if`). | [07](../Part1_Cpp_Basics/lesson07_preprocessor.md) |
| **PWM** | Pulse-width modulation: encoding a value in the width of repeating pulses. | [HW](../hardware/meet_the_braccio.md) |
| **Reference** | Another name (alias) for an existing variable: `int& r = a;` | [13](../Part3_Memory/lesson13_references.md) |
| **Rollover** | `millis()` wrapping back to 0 after ~49.7 days. | [02](../Part1_Cpp_Basics/lesson02_variables.md) |
| **Scope** | The region of code where a name is visible. | [02](../Part1_Cpp_Basics/lesson02_variables.md) |
| **Servo** | A motor with built-in position control that holds a commanded angle. | [HW](../hardware/meet_the_braccio.md) |
| **Shield** | A board that plugs on top of an Arduino. | [HW](../hardware/meet_the_braccio.md) |
| **Sketch** | An Arduino program (`.ino`). | [00](../hardware/first_move.md) |
| **Soft-start** | Gradually switching on servo power to avoid current spikes. | [18](../Part4_Arduino_Libraries/lesson18_bracciov2_deep_dive.md) |
| **SRAM** | The UNO's 2 KB of working memory for variables. | [14](../Part3_Memory/lesson14_dynamic_memory.md) |
| **Stack** | Memory for function calls and local variables, freed automatically. | [14](../Part3_Memory/lesson14_dynamic_memory.md) |
| **`static`** | Local: keeps its value between calls. Member: shared by all objects. | [04](../Part1_Cpp_Basics/lesson04_functions.md), [11](../Part2_OOP/lesson11_methods.md) |
| **`struct`** | Like a class, but members are public by default. | [06](../Part1_Cpp_Basics/lesson06_include_guards.md) |
| **Undefined behaviour** | Code the C++ standard gives no meaning to (out-of-bounds access, null dereference). Anything can happen. | [12](../Part3_Memory/lesson12_pointers.md) |
| **Unit test** | A small automated check of one piece of code's behaviour. | [20](../Part4_Arduino_Libraries/lesson20_deployment.md) |
| **Work envelope** | The space a robot can reach. | [HW](../hardware/meet_the_braccio.md) |
