/*
 * GJB5369: 4.7.2.2
 * using function not by calling is forbidden
 */

bool test_p(void) {
  bool retval = true;
  return retval;
}

void static_p(void) {
  if (test_p) {
    /* ... */
  }
}