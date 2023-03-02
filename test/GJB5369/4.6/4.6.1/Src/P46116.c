/*
 * GJB5369: 4.6.1.16
 * "&&" or "||" used with "=" is forbidden
 */

void static_p(void) {
  bool flag = false;
  unsigned int y = 0u, x = 0u;

  /* "&&" used with "=" is forbidden */
  if (flag && ((x = y) == 0)){
      /* ... */
  }
}