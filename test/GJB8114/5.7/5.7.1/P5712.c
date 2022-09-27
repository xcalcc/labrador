/*
 * GJB8114: 5.7.1.2
 * Formal parameters which are different from real parameters is forbidden
 */

int sum(int p1, short p2);

int main(void) {
  int a = 1, b = 2;
  a = sum(a, b);
  return 0;
}