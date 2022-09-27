/*
 * GJB5369: 4.3.1.6
 * "switch" only containing "default" is forbidden
 */

void static_p(int p_1) {
  int i = p_1;

  /* switch only containing default is forbidden */
  switch (i) {
  default:
    i++;
  }
}