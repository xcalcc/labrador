/*
 * GJB8114: 5.3.1.7
 * Pointers should be initialized as NULL
 */

int main(void) {
  int *x;
  int b = 3;
  x = &b;
  return 1;
}