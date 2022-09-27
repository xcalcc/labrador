#include <stdio.h>

void func(void);

void func(void) {
  int *ptr_int;
  int int_a, int_b;
  int_a = 1;
  ptr_int = (int *) int_a; // convert int to pointer
  int_b = (int) ptr_int;   // convert pointer to type int
}