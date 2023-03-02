/*
 * GJB8114: 5.5.2.1
 * Using goto statement carefully
 */

int main(void) {
  goto L1;
  L1: return 0;
}
