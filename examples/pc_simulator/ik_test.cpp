// Project P5 - test the inverse kinematics on your PC before trusting it with the arm.
// For many target points: IK -> servo angles -> FK -> position. The two positions should match.
// Build: g++ -std=c++17 -Wall -Wextra ik_test.cpp -o ik_test
#include <cmath>
#include <cstdio>
#include "../projects/P5_reach_point/kinematics.h"

int main() {
  // A few hand-picked targets first
  const double targets[][3] = {
      {0, 200, 100},    // straight ahead, 20 cm out, 10 cm up
      {150, 150, 50},   // front-right, low
      {-200, 100, 150}, // front-left
      {0, 250, 0},      // on the table, far
      {0, 450, 50},     // too far: should be unreachable
  };
  std::puts("   target (x, y, z) mm      pitch  base shoulder elbow wrist   FK check (x, y, z)     error");
  for (const auto& t : targets) {
    kin::Angles a;
    int pitch;
    std::printf("  (%6.0f, %6.0f, %6.0f)  ", t[0], t[1], t[2]);
    if (!kin::inverseAnyPitch(t[0], t[1], t[2], a, pitch)) {
      std::puts("  unreachable");
      continue;
    }
    double x, y, z;
    kin::forward(a, x, y, z);
    double err = std::sqrt((x - t[0]) * (x - t[0]) + (y - t[1]) * (y - t[1]) + (z - t[2]) * (z - t[2]));
    std::printf("%5d  %4d %8d %5d %5d   (%6.1f, %6.1f, %6.1f)  %4.1f mm\n",
                pitch, a.base, a.shoulder, a.elbow, a.wrist, x, y, z, err);
  }

  // Then a systematic sweep over the workspace
  int tested = 0, reachable = 0;
  double worst = 0;
  for (int x = -300; x <= 300; x += 25) {
    for (int y = 0; y <= 350; y += 25) {
      for (int z = 0; z <= 300; z += 25) {
        tested++;
        kin::Angles a;
        int pitch;
        if (!kin::inverseAnyPitch(x, y, z, a, pitch)) continue;
        reachable++;
        double fx, fy, fz;
        kin::forward(a, fx, fy, fz);
        double err = std::sqrt((fx - x) * (fx - x) + (fy - y) * (fy - y) + (fz - z) * (fz - z));
        if (err > worst) worst = err;
      }
    }
  }
  std::printf("\nSweep: %d points tested, %d reachable, worst round-trip error %.1f mm\n", tested, reachable, worst);
  std::puts(worst < 8.0 ? "PASS: IK and FK agree (errors come from rounding to whole degrees)"
                        : "FAIL: IK and FK disagree");
  return worst < 8.0 ? 0 : 1;
}
