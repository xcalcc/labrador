#include <stdio.h>

void func(void);

void func(void) {
  void *ptr_void; // void pointer declared
  int *ptr_int;   // int pointer declared
  int int_var;
  ptr_int = &int_var;
  // convert int pointer into void pointer
  ptr_void = (void *) ptr_int;
}