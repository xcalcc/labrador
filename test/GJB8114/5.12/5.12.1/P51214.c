/*
 * GJB8114: 5.12.1.4
 * Comparing(bigger or less) unsigned integers with zero is forbidden
 */

int main(void) {
  unsigned int x = 1, y = 2;
  int flag = 0;
  if (x >= 0) {     // incompliance
    flag = flag + 1;
  }
  if (y <= 0) {     // incompliance
    flag = flag + 1;
  }
  return 0;
}