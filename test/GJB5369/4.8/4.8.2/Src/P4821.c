/*
 * GJB5369: 4.8.2.1
 * avoid using += or -=
 */

void static_p(void) {
  unsigned int x = 1u;
  unsigned int y = 2u;
  unsigned int z = 3u;
  bool flag = false;

  /* avoid using += or -= */
  x += 1;
  z -= y;
}