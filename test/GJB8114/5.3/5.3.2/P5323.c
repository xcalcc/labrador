/*
 * GJB8114: 5.3.2.3
 * Performing arithmatic operator on pointer should be careful
 */

int main(void) {
  int *a, *b;
  int c = 3, d = 4;
  a = &c;
  b = &d;
  c = (int)a + (int)b;
}