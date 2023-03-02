#include <stdio.h>

void func(void);

void func(void) {
  float float_a, float_b;
  int int_var;
  float_a = float_b = 1.0f;
  int_var = (int) float_a + (int) float_b; // float_a and float_b casted to type int
}