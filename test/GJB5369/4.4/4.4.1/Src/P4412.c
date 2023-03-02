/*
 * GJB5369: 4.4.1.2
 * pointer's pointer nested more than two levels is forbidden
 */

void static_p(void) {
  unsigned int arrary[10] = {0};
  unsigned int *p1_ptr, **p2_ptr;
  unsigned int ***p3_ptr;
  unsigned int w;

  p1_ptr = arrary;
  p2_ptr = &p1_ptr;

  /* Pointer's pointer nested more than two levels is forbidden */
  p3_ptr = &p2_ptr;
  w = ***p3_ptr;
}