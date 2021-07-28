#include <stdio.h>

void func(void);

void func(void) {
  int *ptr_int;
  int int_a, int_b;
  int_a = 1;
  ptr_int = &int_a; // ptr_int point to int_a
  int_b = *ptr_int; // *ptr_int assigned to int_b
}