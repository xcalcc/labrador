/*
 * GJB5369: 4.6.1.5
 * assigning negative number to unsigned-variable is forbidden
 */

void static_p(void) {
  unsigned int x = 1u;
  unsigned int y = 2u;
  /* assigning negative number to unsigned-variable is forbidden */
  y = -x;
}