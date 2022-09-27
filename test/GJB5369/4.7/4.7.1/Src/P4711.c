/*
 * GJB5369: 4.7.1.1
 * real and formal parameters' number should be the same
 */

unsigned int test_p(unsigned int p_1, unsigned short p_2) {
  unsigned int result = 0u;
  result = p_1 + p_2;
  return result;
}

void static_p(unsigned int p_1, unsigned short p_2) {
  test_p(1u, 2, 3); /* parameter number is different. */
}