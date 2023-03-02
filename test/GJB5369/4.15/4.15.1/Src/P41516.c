/*
 * GJB5369: 4.15.1.6
 * using volatile type in complex in statement is forbidden
 */

void static_p(void) {
  unsigned int y = 0u, x = 0u, z = 1u;
  unsigned int v = 1u;
  x = v + z / v * y;
}