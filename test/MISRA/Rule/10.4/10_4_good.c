#include <stdio.h>

void func(void);

void func(void) {
  int int_var;
  char char_var;
  // int 1 added to char char_var, permitted
  char_var += 1;
  // int 1 added to int int_var, same type
  int_var += 1;
}