/*
 * GJB5369: 4.8.2.5
 * avoid using useless statements
 */

void static_p(void) {
  unsigned int x = 0u;
  x; /* This statement is useless. */
}