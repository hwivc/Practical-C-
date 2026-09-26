# Project P5: Reach a Point (Inverse Kinematics)

<div class="lesson-meta"><span>⏱ 3–4 h</span><span>🎯 Advanced</span><span>🧩 Uses: math.h, namespaces, references, testing on PC</span><span>🦾 Arm (or Wokwi)</span></div>

!!! abstract "What you'll build"
    Type `0 200 100` and the gripper moves to the point **20 cm in front of the base, 10 cm above the table**. The program
    computes the joint angles itself, using **inverse kinematics (IK)**, the maths behind every robot arm, from factory
    welders to surgical robots.

    **Skills used:** trigonometry in C++ (`<math.h>`), a header shared between PC and Arduino, namespaces (L17),
    references for outputs (L13), testing on the PC before the arm (L20).

---

## Forward vs. inverse

| | Input | Output | Difficulty |
|---|---|---|---|
| **Forward kinematics** (P1) | joint angles | where the gripper is | easy: add up the link vectors |
| **Inverse kinematics** (this project) | where you want the gripper | joint angles | harder: there may be 0, 1 or many answers |

<figure markdown>
![Inverse kinematics geometry](../images/kinematics.svg){ .diagram }
<figcaption>Split the 3-D problem into a top view (base angle) and a side view (a triangle formed by the upper arm, the forearm and the line to the wrist).</figcaption>
</figure>

---

## Step 1: the base angle (top view)

Looking down on the arm, the base must turn to face the target \((x, y)\):

\[
\text{base} = \operatorname{atan2}(y,\ x), \qquad r = \sqrt{x^2 + y^2}
\]

`atan2(y, x)` is like `atan(y / x)`, but it handles every quadrant and \(x = 0\) correctly. \(r\) is how far forward the
gripper must reach, in the arm's own side-view plane. From here on, it's a 2-D problem.

## Step 2: find the wrist

We choose the angle of the hand, its **pitch** \(\varphi\) (−90° = pointing straight down). The wrist joint is then one hand-length
\(L_3\) "behind" the tip:

\[
r_w = r - L_3\cos\varphi, \qquad z_w = z - H - L_3\sin\varphi
\]

(\(H\) is the height of the shoulder axis above the table.)

## Step 3: solve the triangle (law of cosines)

The shoulder, the elbow and the wrist form a triangle with sides \(L_1\), \(L_2\) and \(d = \sqrt{r_w^2 + z_w^2}\). The **law of cosines**
gives the elbow bend \(\theta_2\):

\[
\cos\theta_2 = \frac{r_w^2 + z_w^2 - L_1^2 - L_2^2}{2 L_1 L_2}
\]

- If the right-hand side is **greater than 1 or less than −1**, the wrist is out of reach, and there's no solution.
- Otherwise there are **two** solutions, \(\pm\theta_2\) ("elbow up" and "elbow down"). We pick elbow-up (\(\theta_2 < 0\)), which keeps the
  arm clear of the table.

Then the shoulder angle:

\[
\theta_1 = \operatorname{atan2}(z_w, r_w) - \operatorname{atan2}(L_2\sin\theta_2,\ L_1 + L_2\cos\theta_2)
\]

## Step 4: convert to servo angles

With the conventions from Project P1 (servo 90 = straight in line):

\[
\text{shoulder} = \theta_1, \qquad \text{elbow} = 90^\circ + \theta_2, \qquad \text{wrist} = 90^\circ + \big(\varphi - (\theta_1 + \theta_2)\big)
\]

Finally, round to whole degrees and **check every joint's limits**. A mathematically valid answer can still be
physically impossible.

## Step 5: choose the pitch automatically

Many pitches can reach the same point. The code tries every pitch from −90° to +30° in 5° steps and keeps the most
*comfortable* solution, the one whose joints are closest to the middle of their ranges.

---

## The code: one header, two platforms

`kinematics.h` uses only `<math.h>`, so **the same file** compiles on your PC for testing and on the Arduino for real:

```cpp title="kinematics.h" linenums="1"
--8<-- "examples/projects/P5_reach_point/kinematics.h"
```

!!! note "C++ details worth noticing"
    - Everything is in `namespace kin`, so `kin::L1` can't clash with anything else (Lesson 17).
    - Functions defined in a header are marked `inline`, so the header can be included in several `.cpp` files without
      breaking the One Definition Rule (Lesson 05).
    - Results come back through **reference parameters** (`Angles& out`, `double& x`), while the `bool` return says whether it worked (Lesson 13).
    - `lround` rounds to the nearest integer. A plain `(int)` cast would always round **down**.
    - On the UNO, `double` is only 32 bits (Lesson 02). That's plenty for millimetre accuracy.

---

## Step 6: test on the PC first

Before sending IK output to a real arm, prove it's right. For thousands of points: IK → angles → FK → position. If the
maths is correct, you get back where you started:

```cpp title="ik_test.cpp" linenums="1"
--8<-- "examples/pc_simulator/ik_test.cpp"
```

```bash
cd examples/pc_simulator
g++ -std=c++17 -Wall -Wextra ik_test.cpp -o ik_test
./ik_test
```

```text
   target (x, y, z) mm      pitch  base shoulder elbow wrist   FK check (x, y, z)     error
  (     0,    200,    100)    -55    90       99    22     4   (   0.0,  199.4,   99.6)   0.7 mm
  (   150,    150,     50)    -65    45       86    18    11   ( 150.2,  150.2,   49.7)   0.4 mm
  (  -200,    100,    150)    -40   153      100    30    10   (-199.1,  101.4,  149.6)   1.8 mm
  (     0,    250,      0)    -60    90       76     3    41   (   0.0,  250.4,    0.1)   0.4 mm
  (     0,    450,     50)    unreachable

Sweep: 4875 points tested, 4272 reachable, worst round-trip error 7.1 mm
PASS: IK and FK agree (errors come from rounding to whole degrees)
```

The small errors come from rounding angles to **whole degrees**: 1° at the end of a 445 mm arm is about 8 mm. Real servos
have a similar resolution, so this is as precise as the hardware can be.

---

## Step 7: the Arduino sketch

```cpp title="P5_reach_point.ino" linenums="1"
--8<-- "examples/projects/P5_reach_point/P5_reach_point.ino"
```

!!! warning "Before the first real run"
    1. **Measure your arm.** Update `BASE_HEIGHT`, `L1`, `L2`, `L3` in `kinematics.h` with a ruler (from joint axis to joint axis).
    2. **Check directions.** Command `0 200 300` (in front, high up). If the arm leans *backward*, set `MIRROR_SHOULDER = true`,
       and check the elbow and wrist the same way.
    3. **Start high.** Test points with `z ≥ 150` first, and only then approach the table.
    4. **Put a target on the table.** Mark a cross at (0, 200) and command `0 200 20`. Measure how close the gripper gets.

A session:

```text
Type a target as: x y z   (mm; y is straight ahead, z is up)
> target 0 200 100
pitch -55 -> base 90, shoulder 99, elbow 22, wrist 4
> target 150 150 50
pitch -65 -> base 45, shoulder 86, elbow 18, wrist 11
> target 0 450 50
Unreachable (too far, too close, behind the arm, or outside a joint's limits)
```

---

## Extensions

1. **Straight lines.** Move the gripper from A to B in a *straight line* by computing IK for 20 points along the line
   and visiting them in order. (Joint-space blending from Lesson 13 moves in a *curve*.)
2. **Draw a square.** With a pen taped in the gripper, draw a 5 cm square on paper at z ≈ 0.
3. **Pick by coordinates.** Combine with P2: `PICK 100 150` computes the approach, descend and lift poses automatically.
4. **Calibrate the model.** Command five points, measure where the gripper really went, and adjust `L1`–`L3` to minimise the error.
5. **Vision.** Use a webcam and Python/OpenCV to find a coloured ball, convert its pixel position to millimetres, and
   send `x y z` over serial (P3's Python script) so the arm points at it.

??? success "Hint for extension 1"
    ```cpp
    void moveStraight(long x0, long y0, long z0, long x1, long y1, long z1) {
      const int N = 20;
      for (int i = 1; i <= N; i++) {
        long x = x0 + (x1 - x0) * i / N;
        long y = y0 + (y1 - y0) * i / N;
        long z = z0 + (z1 - z0) * i / N;
        reach(x, y, z);
        arm.waitUntilStopped();
      }
    }
    ```

---

## Checklist

- [ ] `ik_test` passes on my PC
- [ ] I measured my arm's link lengths and updated `kinematics.h`
- [ ] Directions checked (mirror flags set if needed)
- [ ] The gripper reaches a marked point within ~1 cm
- [ ] At least one extension

🏆 **Congratulations, you've completed the course!** You wrote C++ from "Hello" to inverse kinematics, built and tested your
own library, and made a real robot arm do what *you* decided. Explore [Resources & Further Learning](../Appendix/resources.md) for where to go next.

## Further reading

- [Robot Academy (Peter Corke): Inverse kinematics for a 2-joint arm using geometry](https://robotacademy.net.au/lesson/inverse-kinematics-for-a-2-joint-robot-arm-using-geometry/)
- [Wikipedia: Law of cosines](https://en.wikipedia.org/wiki/Law_of_cosines)
- [Modern Robotics (Lynch & Park), free textbook and videos](https://hades.mech.northwestern.edu/index.php/Modern_Robotics)
