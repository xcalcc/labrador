/*
 * GJB5369: 4.14.1.1
 * avoid comparing two real numbers
 */

void static_p(void) {
  float f1, f2;
  f1 = 1.01f;
  f2 = 2.01f;

  /* avoid comparing two real numbers */
  if (f1 == f2) {
  }

  /* avoid comparing two real numbers */
  if (f1 == 0.0f) {
    f1 = f1 + 0.01f;
  }
}