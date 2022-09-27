/*
 * GJB5369: 4.12.2.2
 * avoid using unnecessary cast
 */

void static_p(void) {
  int sx, sy = -10;

  /* Unnecessary cast */
  sx = (int)sy + 1;
}