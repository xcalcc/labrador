/*
 * GJB8114: 5.3.1.3
 * Assigning to pointer parameters is forbidden
 */

int func(int *p) {
  int i = 3;
  p = &i;
  return i;
}
