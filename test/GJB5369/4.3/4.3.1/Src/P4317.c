/*
 * GJB5369: 4.3.1.7
 * "case" statement without "break" is forbidden
 */

void static_p(int p_1) {
  int i = 0, j = 0;
  switch (p_1) {
  case 0:
    j = 0;  /* "case" statement without "break" is forbidden */
  case 1:
    j = i;
    break;
  default:
    i = j + 1;
  }
}