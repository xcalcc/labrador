/*
 * GJB5369: 4.12.1.1
 * cast on pointer is forbidden
 */

void static_p(void) {
  unsigned short s = 0;
  unsigned int *p1_ptr;

  /* cast on pointer is forbidden */
  p1_ptr = (unsigned int *)s;
}