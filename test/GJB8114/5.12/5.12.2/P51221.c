/*
 * GJB8114: 5.12.2.1
 * Constant value should stay at left side of the compare operator
 */

int main(void) {
  int a = 2;

  if (a == 2) { // incompliance
    return 1;
  }
  if (2 > a) {
    return 2;
  }
}