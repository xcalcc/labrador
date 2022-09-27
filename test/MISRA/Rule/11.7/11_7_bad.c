#include <stdio.h>

void func(void);

void func(void) {
  char char_a, char_b;
  char *ptr_char;
  char_a = 'a';
  // convert char_a to char pointer
  ptr_char = (char *) char_a;
  // convert ptr_char to char type
  char_b = (char) ptr_char;
}