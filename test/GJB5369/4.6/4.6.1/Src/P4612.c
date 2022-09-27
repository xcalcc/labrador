/*
 * GJB5369: 4.6.1.2
 * using array out of boundary is forbidden
 */

void static_p(void) {
  unsigned int a[4];

  /* out of boundary */
  a[5] = 1;
}