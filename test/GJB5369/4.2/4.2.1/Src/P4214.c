/*
 * GJB5369: 4.2.1.4
 * logic expression should be enclosed in parentheses
 */

void static_p(void) {
  bool flag = true;
  unsigned int y = 0u, x = 0u, z = 1u;

  /* The logic expression should be enclosed in parentheses */
  if (x < 0 || z + y != 0 && !flag) {
      flag = false;
  }
}