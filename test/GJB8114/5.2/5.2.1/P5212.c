/*
 * GJB8114: 5.2.1.2
 * if-else body should be enclosed with brace
 */

int main(void) {
  int i = 1;
  if (i == 2) i++;
  else if (i > 3) i++;
  else i--;
}