/*
 * GJB8114: 5.1.2.4
 * Variables should be declared at the beginning of function body
 */

int main(void) {
  int a = 3;
  a = 2;
  int b = 5;
  a = a + b;
  return 0;
}