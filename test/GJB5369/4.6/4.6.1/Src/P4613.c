/*
 * GJB5369: 4.6.1.3
 * bit shift used on signed-number is forbidden
 */

void static_p(void) {
  int b = 1;
  b >>= 1;  /* bit-shift on signed-number is forbidden */
}