/*
 * GJB5369: 4.2.1.3
 * if/else block must be enclosed in braces
 */

int static_p(int p_1, int p_2) {
  int i = 1;
  int j = 2;
  if (p_1 > 0) {
    i = i - 1;
  } else    /* else block must be enclosed in braces */
    i = i + 1;

  if (p_2 > 0) {
    j = j + p_2;
  } else if (p_2 < 0) {
    j = j - p_2;
  } else {
    j = i;
  }

  return i;
}
