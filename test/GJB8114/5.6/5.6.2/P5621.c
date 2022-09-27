/*
 * GJB8114: 5.6.2.1
 * Bitwise operating on signed integer should be careful
 */

int main(void) {
  int a = 10;
  a = a >> 1;
  return 0;
}