/*
 * GJB5369: 4.4.1.1
 * assigning the parameter pointer to
 * the procedure pointer is forbidden
 */

unsigned int *static_p(unsigned int *p1_ptr) {
  static unsigned int w = 10u;
  p1_ptr = &w;
  return &w;
}