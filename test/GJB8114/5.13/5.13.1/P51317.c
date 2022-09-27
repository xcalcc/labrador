/*
 * GJB8114: 5.13.1.7
 * Redefining exist variables in function is forbidden
 */

int main(void) {
  int a;
  if (a == 2) {
    int a = 3;
  }
  return 2;
}