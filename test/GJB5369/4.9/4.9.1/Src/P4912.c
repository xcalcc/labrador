/*
 * GJB5369: 4.9.1.2
 * return in a void type function is forbidden
 */

void static_p(unsigned int p_1, unsigned int p_2) {
  unsigned int result;
  result = p_1 + p_2;

  /* returnn in a void type function is forbidden */
  return result;
}