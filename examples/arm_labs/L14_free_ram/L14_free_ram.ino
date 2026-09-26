// Lesson 14 - Watching the UNO's 2 KB of RAM.
#include <BraccioV2.h>

Braccio arm;

// Free bytes between the top of the heap and the bottom of the stack (AVR only).
#ifdef __AVR__
extern unsigned int __heap_start;
extern void* __brkval;
int freeMemory() {
  int topOfStack;   // a local variable lives at the current top of the stack
  return (int)&topOfStack - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
#else
int freeMemory() { return -1; }   // not available on this board / in the simulator
#endif

void report(const __FlashStringHelper* label) {
  Serial.print(label);
  Serial.print(F(" free RAM: "));
  Serial.println(freeMemory());
}

// A function with a big local array: the stack grows while it runs.
void bigLocalBuffer() {
  int samples[200];                       // 400 bytes on the stack!
  for (int i = 0; i < 200; i++) samples[i] = i;
  report(F("inside bigLocalBuffer (400-byte array):"));
  Serial.print(F("  (checksum "));
  Serial.print(samples[199]);             // use the array so it isn't optimised away
  Serial.println(F(")"));
}

void setup() {
  Serial.begin(9600);
  report(F("at start of setup:"));

  arm.begin();
  report(F("after arm.begin():"));

  bigLocalBuffer();
  report(F("after bigLocalBuffer returned:"));   // the stack shrank again
}

void loop() {
  // The F() macro keeps this long message in FLASH instead of copying it into RAM.
  Serial.println(F("Waving. This long message costs zero bytes of RAM thanks to the F() macro."));
  arm.setOneAbsolute(WRIST, 60);
  arm.safeDelay(700);
  arm.setOneAbsolute(WRIST, 120);
  arm.safeDelay(700);
  report(F("in loop:"));
}
