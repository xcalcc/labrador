#include <stdint.h>

static char a[10];

void func(void);

void func(void) {
  int i;
  char *p;
  // same effect achieved by other mean
  p = &a[0];
  for (i = 0; i < 10; ++i) {
    *p++ = i;
  }
}