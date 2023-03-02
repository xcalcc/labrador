#include <stdio.h>

#define MAXIMUM 0xffffffff // maximum value of unsigned int

void func(void);

void func(void) {
  unsigned long long long_long_var;
  // 0xffffffff assigned to long_long_var
  long_long_var = MAXIMUM;
  // expression long_long_var + 1 is not a constant expression
  long_long_var = long_long_var + 1;
}