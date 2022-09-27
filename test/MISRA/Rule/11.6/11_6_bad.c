#include <stdio.h>

void func(void);

void func(void) {
  void *ptr_void;
  int int_var;
  // convert int_var to void pointer
  ptr_void = (void *) int_var;
}