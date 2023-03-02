/*
 * GJB5369: 4.15.2.2
 * using ## and # carefully in macro
 */

#include <stdio.h>

#define FillArray(x, y) sprintf(n[##x]."%s", &y)

void static_p(void) {
  char n[3][10];
  FillArray(0, "TEST0");
  FillArray(1, "TEST1");
  FillArray(2, "TEST2");
}