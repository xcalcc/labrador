/*
 * GJB5369: 4.6.2.3
 * different types of variable mixed operation should be carefully
 */

void static_p(void) {
  int sx = -10;
  unsigned short usi = 1;
  unsigned int ui = 2u;
  float f1 = 2.0f;
  double db1 = 3.0;
  ui = sx + 2u;
  usi = ui + 1u;
  f1 = db1 * f1; /* should be f1=(float)db1*f1 */
}