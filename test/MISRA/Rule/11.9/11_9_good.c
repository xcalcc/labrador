#include <stdio.h>

void func(void);

void func(void) {
  int *int_ptr = NULL; // ptr_int point to NULL
  // NULL appear as an operand with the other operand a pointer
  if (int_ptr != NULL) {
    *int_ptr = NULL;
  }
}