# ArmPoses

Named poses and smooth pose playback for the Tinkerkit Braccio arm. Written in
[Lesson 17](https://theafricanjiant.github.io/Practical-C-/Part4_Arduino_Libraries/lesson17_library_structure/)
of *Practical C++ with the Braccio Arm*.

## Install

Copy this folder into your sketchbook's `libraries/` folder (e.g. `Documents/Arduino/libraries/ArmPoses`)
and restart the Arduino IDE. Requires the **BraccioV2** library (Library Manager).

## Use

```cpp
#include <BraccioV2.h>
#include <ArmPoses.h>

Braccio arm;
PosePlayer player(arm);

void setup() { arm.begin(); }

void loop() {
  player.moveTo(Poses::PARK, 2000);   // glide there in 2 s, all joints arriving together
  player.moveTo(Poses::HOME, 2000);
}
```

See `examples/PoseDance`.
