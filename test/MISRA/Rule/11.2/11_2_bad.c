#include <stdio.h>

union uni; // incomplete type declared
void func(void);

void func(void) {
  union uni *ptr_union;
  int *ptr_int;
  // ptr_int converted to pointer to incomplete type
  ptr_union = (union uni *) ptr_int;
  // ptr_union converted to pointer to int
  ptr_int = (int *) ptr_union;
}