#include <stdio.h>

void func(void);

void func(void) {
  int int_var;
  char char_var;
  // float 1.0f added to char char_var, not permitted
  char_var += 1.0f;
  // float 1.0f added to int int_var, differnt type
  int_var += 1.0f;
}