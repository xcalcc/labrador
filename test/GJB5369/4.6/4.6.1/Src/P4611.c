/*
 * GJB5369: 4.6.1.1
 * "=" used in non-assignment statement is forbidden
 */

void static_p(void) {
  unsigned int z = 0u, x = 0u;
  bool flag = true;

  /* "=" used in non-assignment statement is forbidden */
  if (flag = false) {
    z = x - 1u;
  }
}