/*
 * GJB8114: 5.9.1.1
 * Condition variable of for-loop must use local variable
 */

int g = 0;
int main(void) {
  int i = 10;
  for (g = 0; g < 10; g++) {
    i = i - 1;
  }
  return 0;
}