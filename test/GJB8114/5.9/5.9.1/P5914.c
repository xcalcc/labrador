/*
 * GJB8114: 5.9.1.4
 * Infinite loop must use while(1)
 */

int main(void) {
  for (;;) {
    return 2;
  }
  return 3;
}
