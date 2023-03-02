/*
 * GJB5369: 4.6.1.9
 * The type of assignment must match
 */

void static_p(void) {
  float f1 = 2.0f;
  double db1 = 3.0;
  f1 = db1; /* The type of assignment must match */
}