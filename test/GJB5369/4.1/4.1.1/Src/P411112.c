/*
 * GJB5369: 4.1.1.12
 * Macro which is unlike a function is forbidden
 */

#define IF_X(x) if (x) {    /* the macro is unlike a function */

void static_p(void) {
  bool test = true;
  IF_X(test) test != test;
  }
}