#include <stdio.h>

void func(void);

void func(void) {
  char char_a, char_b;
  char *ptr_char;
  char_a = 'a';
  // ptr_char point to char_a
  ptr_char = &char_a;
  // value in ptr_char assigned to char_b
  char_b = *ptr_char;
}