# Troubleshooting

Find your symptom, then work through the checks **in order**: cheapest and most likely first.

!!! danger "First, be safe"
    If the arm is doing something unexpected, **unplug the 5 V adapter** first and think second. The USB cable alone
    can't move the motors.

---

## Upload & IDE problems

??? question "No port appears under Tools → Port"
    1. Try another USB cable. Many cheap cables carry power only, no data.
    2. Try another USB port (avoid unpowered hubs).
    3. Clone boards with a **CH340** USB chip need a driver: [SparkFun CH340 guide](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all).
    4. Linux: add yourself to the `dialout` group (`sudo usermod -a -G dialout $USER`, then log out and in).

??? question "`avrdude: stk500_recv(): programmer is not responding`"
    - Wrong board or port selected. Check *Tools → Board = Arduino Uno* and the port.
    - The Serial Monitor or another program (a Python script, Cura…) has the port open. Close it.
    - Something is connected to pins 0/1 (RX/TX). Disconnect it during upload.

??? question "`fatal error: BraccioV2.h: No such file or directory`"
    - Install **BraccioV2** via *Sketch → Include Library → Manage Libraries*.
    - For course libraries (`MyBraccio`, `ArmPoses`): copy the folder into `Documents/Arduino/libraries/` and
      **restart the IDE**. The folder must directly contain `library.properties` and `src/`, not another nested folder
      (a common result of unzipping).

??? question "The sketch compiles on my friend's computer but not mine"
    Compare IDE versions, board package versions (*Tools → Board → Boards Manager*) and library versions. Turn on
    *verbose output* (Lesson 16) to see which library folder is actually being used, and look for "Multiple libraries were found".

---

## The arm doesn't move

??? question "Nothing moves at all"
    1. Is the **5 V adapter** plugged into the shield? USB alone can't power the servos.
    2. Did you wait for the soft-start? `begin()` takes about **6–8 seconds**.
    3. Is it a **Braccio Shield V4 or newer**? BraccioV2 and MyBraccio switch power on through pin 12, and older shields
       don't have that circuit. Use the official Arduino Braccio library instead.
    4. With BraccioV2: are you calling `update()` or `safeDelay()`? `setAllAbsolute()` only sets targets. `delay()` does *not* move the arm.
    5. Are the servo cables pushed fully onto the M1–M6 headers, in the right orientation?

??? question "One joint doesn't move"
    - Swap its cable to another header (and change the pin in a test sketch) to tell a bad servo from a bad header.
    - Check its limits: a target inside a tiny range (e.g. min = max) won't move.
    - Check its speed/delta: `setDelta(joint, 0)` freezes a joint in BraccioV2.

??? question "The arm moves for about 30 seconds, then stops responding"
    Almost certainly a timing variable declared as `int`: `int endTime = millis() + 3000;` overflows after 32.7 s on the
    UNO. Use `unsigned long` and `millis() - start < duration` ([Lesson 02](../Part1_Cpp_Basics/lesson02_variables.md)).

---

## The arm moves badly

??? question "The Arduino resets (start-up messages repeat) when the arm moves"
    The supply voltage is dropping under load (a *brown-out*):
    - Use the original **5 V, 4–5 A** adapter, not a phone charger.
    - Make sure the soft-start is running (don't bypass `begin()`).
    - Reduce the speed: many joints accelerating at once draw a lot of current.

??? question "A joint jitters or oscillates around its target"
    - **BraccioV2 with `setDelta > 1`**: the overshoot bug ([Lesson 18](../Part4_Arduino_Libraries/lesson18_bracciov2_deep_dive.md#bug-hunt)).
      Use delta 1, or MyBraccio.
    - Noisy analog input (potentiometers): average several readings and ignore changes of 1–2°.
    - A servo at the mechanical end of its travel: move the limit a few degrees inward.

??? question "A servo buzzes, hums or gets hot"
    It's pushing against something and can't reach its angle: the gripper squeezing an object or itself (keep it ≤ 73,
    and lower for objects), a joint pressed against the table, or a limit set beyond the mechanical range. **Cut the power**
    and fix the limit.

??? question "The arm jumps violently at start-up"
    - With BraccioV2 `begin(false)`: you must call `setAllNow(...)` immediately (bug 5). Use `begin()` or MyBraccio's `begin(pose)`.
    - The start pose is far from where the arm was resting. That's normal on the first power-up after moving it by hand.
      Park the arm (`90 45 180 180 90 10`) before switching off.

??? question "Joints move the wrong way / poses look mirrored"
    Arms are assembled differently. Record your directions ([Lesson 00](../hardware/first_move.md#step-5-find-the-directions-on-your-arm))
    and adapt: mirror an angle with `180 - angle`, or use the `MIRROR_*` flags in Project P5.

??? question "90° is not straight"
    Calibrate: find the true centre with the jog controller ([Lesson 12](../Part3_Memory/lesson12_pointers.md)) and set it with
    `setJointCenter` (BraccioV2) or use your `JointConfig` values ([Lesson 09](../Part2_OOP/lesson09_constructors.md)).

---

## Program behaves strangely

??? question "Random resets, garbage in the Serial Monitor, variables changing by themselves"
    Memory trouble ([Lessons 14–15](../Part3_Memory/lesson14_dynamic_memory.md)):
    - Too little free RAM: check *"Global variables use…"*, wrap strings in `F()`, and avoid `String`.
    - Writing past the end of an array (`i <= 6` on a 6-element array).
    - Large local arrays or recursion overflowing the stack.

??? question "Serial Monitor shows gibberish"
    The baud rate in the monitor doesn't match `Serial.begin(...)`. Both must be the same (9600 or 115200).

??? question "Serial commands are ignored or run twice"
    - Set the Serial Monitor's line ending to **Newline** for the P3/P4/P5 projects.
    - `'\r'` characters from *Both NL & CR* are ignored by the course code, but your own parser may not ignore them.

??? question "It works in Wokwi but not on the arm (or vice versa)"
    The simulator has no load, no power limits and no collisions. Check power, cable seating and the mechanical limits, then
    test at speed 1 on the real arm.

---

## Still stuck?

1. Make the **smallest sketch** that shows the problem.
2. Write down: what you expected, what happened, and what you already tried.
3. Ask on the [Arduino Forum](https://forum.arduino.cc/), or [open an issue](https://github.com/TheAfricanJiant/Practical-C-/issues)
   on this course's repository with that information.
