/*
 * GJB5369: 4.8.2.3
 * avoid using continue statement
 */

void static_p(int p_1) {
  int i = p_1;
  int x = 0;
  while (i != 0) {
    i--;
    if (x == 0) {
      continue; /* avoid using continue statement */
    }
  }
}