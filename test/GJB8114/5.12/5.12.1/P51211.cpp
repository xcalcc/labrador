/*
 * GJB8114: 5.12.1.1
 * Comparing logic values is forbidden
 */

int main(void) {
  bool a1, a2;
  int r = 100, h = 500;
  a1 = (r > 100);
  a2 = (h < 23);
  if (a1 > a2) {
    r = 2;
  }
  if (a2 < a1) {
    h = 3;
  }
  return 2;
}