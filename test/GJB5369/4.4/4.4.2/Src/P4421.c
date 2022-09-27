/*
 * GJB5369: 4.4.2.1
 * comparing two pointer should be careful
 */

void static_p(unsigned int *p1_ptr, unsigned int *p2_ptr) {
  /* comparing two pointer should be careful */
  if (p1_ptr > p2_ptr) {
  }
}