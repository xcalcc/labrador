/*
 * GJB5369: 4.3.1.2
 * 'else' must be used in the "if...else if" statement
 */

void static_p(void) {
  unsigned int x = 2u;
  if (x == 2u) {

  } else if (x == 3u) {
  } /* else statement is missing */
}