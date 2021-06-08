/*
 * GJB8114: 5.6.1.12
 * Divided by zero is forbidden in division
 */

void f(int i) {
  int b = i / 0;
  int c = i % 0;
}