/*
 * GJB8114: 5.9.2.3
 * Using multiple continue statements in loop should be careful
 */

int main(void) {
  while (1) {
    continue;
    continue;
  }
  return 0;
}