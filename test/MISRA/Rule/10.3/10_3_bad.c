#include <stdio.h>

void func(void);

void func(void) {
  unsigned int a = -1; // signed value assigned to a
  int b = 'b'; // character assigned to b
  short int short_a;
  long int long_b;
  short_a = long_b; // c assigned to narrower essential type object d
}