/*
 * GJB8114: 5.3.1.1
 * Pointer nested more than 2 levels is forbidden
 */

int main(void) {
  int *p1 = 0, **p2 = 0;
  unsigned int ***p3 = 0; // incompliance

  return 0;
}