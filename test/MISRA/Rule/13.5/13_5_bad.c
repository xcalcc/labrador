#include <stdio.h>

static int int_a;

void func(void);

int func2(void);

bool return_true(void);

void func(void) {
  int int_b;

  // int_a is not modified as intended
  if (return_true() || int_b == func2());
  // more code that may read int_a
  // ...
}

int func2(void) {
  int_a++; // side effect: int_a is modified
  return int_a;
}

bool return_true(void) {
  return true;
}