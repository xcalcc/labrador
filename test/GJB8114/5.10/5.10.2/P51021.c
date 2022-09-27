/*
 * GJB8114: 5.10.2.1
 * Rounding need to be taken in account when convert float to integer
 */

int main(void) {
  int a = 10;
  float b = 0.2;
  a = b;
  return 0;
}