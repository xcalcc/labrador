/*
 * GJB8114: 5.6.1.10
 * Performing logic-not on ingeter literal is forbidden
 */

int main(void) {
  int i = 0, j = 0;
  if (i == !1) {
    j = 1;
  }
  return 0;
}