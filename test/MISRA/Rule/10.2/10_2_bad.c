#include <stdio.h>

int func(void);

int func(void) {
  // essentially character type declared
  char char_var;
  int numb;
  char_var = '9';
  // the result of numa + '0' do not make sense
  numb = char_var + '0';
}