/*
 * GJB5369: 4.6.1.17
 * bit-wise operation on bool is forbidden
 */

void static_p(void) {
  unsigned int x = 1u;
  bool flag = false;

  /* bit-wise operation on bool is forbidden */
  if ((flag | (x != 0)) == false) {
    /* ... */
  }
}