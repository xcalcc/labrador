/*
 * GJB8114: 5.12.1.2
 * Comparing pointers is forbidden
 */

int fsub(int *a, int *b) {
  int sub =0;
  if (a > b) {
    sub = (*b) - (*a);
  } else if (a <b) {
    sub = (*b) - (*a);
  } else {
    sub = 0;
  }

  return sub;
}