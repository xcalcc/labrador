/*
 * GJB8114: 5.7.1.1
 * Overriding functions in standard libraries is forbidden
 */

int printf(int a, int b) {
  return (a > b) ? a : b;
}

int main(void) {
  int ret;
  ret = printf(2, 3);
  return 0;
}