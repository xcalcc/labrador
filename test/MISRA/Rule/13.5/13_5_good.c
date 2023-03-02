#include <stdio.h>

static int int_a;

void func(void);

int func2(void);

bool return_true(void);

void func(void) {
  int int_b;

  // right operand do not have side effect
  if (return_true() || int_b == func2());
  // more code that may read int_a
  // ...
}

int func2(void) {
  return int_a + 1; // do not have side effect
}

bool return_true(void) {
  return true;
}