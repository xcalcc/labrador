/*
 * GJB5369: 4.2.1.2
 * the loop must be enclosed in braces
 */

int static_p(int p_1) {
  int j = 10;
  int k = 0;
  /* ... */
  for (k = 0; k < 10; k = k + 1)    // the loop must be enclosed in braces
    j--;
  return j;
}