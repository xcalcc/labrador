/*
 * GJB8114: 5.6.1.4
 * Bitwise operator within logic statement is forbidden
 */

int main(void) {
  int i = 3, j = 4;
  if ((i == 4) | (j == 6)) {
    return 7;
  }
  return 0;
}
