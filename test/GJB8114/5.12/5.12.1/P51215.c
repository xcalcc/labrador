/*
 * GJB8114: 5.12.1.5
 * Comparing unsigned number with signed number is forbidden
 */

int main(void) {
  unsigned int x;
  int y, i;
  x = 2;
  y = -2;
  if (y < x) {  // incompliance
    i = 0;
  } else {
    i = 1;
  }
  return 1;
}