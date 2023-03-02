/*
 * GJB8114: 5.6.2.6
 * Using comma operator carefully
 */

int main(void) {
  int a, b, c;
  a = (b = 1, c = 2);
  return 0;
}