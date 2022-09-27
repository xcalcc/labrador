#include <stdio.h>

void func(void);

void func(void) {
  char char_var;
  bool bool_var;
  // expression 1.0f cast to an narrower type char
  char_var = (char) 1.0f;
  // char_var cast to narrower type bool
  bool_var = (bool) char_var;
}