#include <stdio.h>

void func(void);

void func(void) {
  int *int_ptr = 0; // 0 assigned to ptr_int
  // 0 appear as an operand with the other operand a pointer
  if (int_ptr != 0) {
    *int_ptr = NULL;
  }
}