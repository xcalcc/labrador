/*
 * GJB5369: 4.3.1.4
 * "default" statement should be used in the "switch" statement
 */

void static_p(int p_1) {
  int i = 0, j = 0;
  switch (p_1) {
  case 0:
    j = 0;
    break;
  case 1:
    j = 1;
    break;

    /* "switch" without "default" is forbidden */
  }
}