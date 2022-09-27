#include <stdio.h>

void func(void);

void func(void) {
  bool bool_var;
  bool_var = true;
  // == is not an assignment operator
  while (bool_var == true) {
    // some code that change bool_var
    // ...
  }

}