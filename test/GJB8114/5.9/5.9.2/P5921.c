/*
 * GJB8114: 5.9.2.1
 * Using infinite loop carefully
 */

int main(void) {
  for (;;) {
    int a = 2;
    break;
  }

  while (1) {
    return 1;
  }
  return 0;
}