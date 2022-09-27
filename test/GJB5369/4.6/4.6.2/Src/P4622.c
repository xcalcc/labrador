/*
 * GJB5369: 4.6.2.2
 * "sizeof()" should be used carefully
 */

void static_p(void) {
  unsigned int x = 1u;
  unsigned int y = 2u;
  int a = 3;

  a = sizeof(x = y); /* sizeof won't caculate the expr */
}