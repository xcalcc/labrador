#include <stdio.h>

void func(void);

void func(void) {
  int *ptr_int;                 // pointer to int declared
  char *ptr_char;               // pointer to char declared
  ptr_int = (int *) ptr_char; // converted to int type pointer
}