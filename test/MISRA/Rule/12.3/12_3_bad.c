#include <stdint.h>

static char a[10];

void func(void);

void func(void) {
  int i;
  char *p;
  // comma operator hard the readability of code
  for (i = 0, p = &a[0]; i < 10; ++i, *p++ = i);
}