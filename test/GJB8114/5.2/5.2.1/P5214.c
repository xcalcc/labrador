/*
 * GJB8114: 5.2.1.4
 * Logic expressions should be enclosed with parentheses
 */

int main(void) {
  int i = 1, j = 2, k = 3;
  if ((i == 0) || (j == 2) && (k == 3)) {
    j = j - 2;
  }
  return 0;
}