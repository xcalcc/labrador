#include <stdio.h>

void func(void);

void func(void) {
  int *ptr_int;    // pointer to int declared
  char *ptr_char;  // pointer to char declared
  *ptr_char = NULL;
  *ptr_int = NULL; // pointers point to null
}