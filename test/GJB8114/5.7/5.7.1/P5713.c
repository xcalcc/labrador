/*
 * GJB8114: 5.7.1.3
 * Formal parameters' number should be the same with real paremters
 */

int sum(int p1, short p2);

int main(void) {
  int a = 1, b = 2;
  a = sum(a, b, a);
  return 0;
}