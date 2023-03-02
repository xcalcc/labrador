/*
 * GJB5369: 4.13.1.4
 * value used before init is forbidden
 */

int static_p(void) {
  unsigned int x, y;
  return x + y;
}