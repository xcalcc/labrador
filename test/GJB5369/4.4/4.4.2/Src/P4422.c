/*
 * GJB5369: 4.4.2.2
 * using pointer in the algebrac operation should be careful
 */

void static_p(void) {
  unsigned int w;
  unsigned int array[5];
  unsigned int *p1_ptr;
  p1_ptr = array;

  /* using pointer in the algebrac operation should be careful */
  w = *(p1_ptr + 8);
}