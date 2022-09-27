#include <stdio.h>

#define MAXIMUM 0xffffffff // maximum value of unsigned int

void func(void);

void func(void) {
  unsigned long long long_long_var;
  // the value of MAXIMUM + 1 lead to wrap-around
  long_long_var = MAXIMUM + 1;
}