/*
 * GJB8114: 5.3.1.4
 * Returning local variables address from functions is forbidden
 */

int *f(void) {
  int a = 10;
  return &a;
}