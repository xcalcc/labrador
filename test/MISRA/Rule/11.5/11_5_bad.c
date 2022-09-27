#include <stdio.h>

void func(void);

void func(void) {
  void *ptr_void; // void pointer declared
  int *ptr_int;   // int pointer declared
  int int_var;
  ptr_void = &int_var;
  // convert void pointer into int pointer
  ptr_int = (int *) ptr_void;
}