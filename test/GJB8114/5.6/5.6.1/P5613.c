/*
 * GJB8114: 5.6.1.3
 * Assignment in logic statements is forbidden
 */


int main(void) {
  int i;
  if (i = 3) {
    return i;
  }
  return 1;
}