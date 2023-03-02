/*
 * GJB8114: 5.10.2.3
 * Convert int to shorter int carefully
 */

int main(void) {
  int a = 3;
  short b;
  b = a;
  b = (short)a;
  return 0;
}
