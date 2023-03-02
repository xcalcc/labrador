/*
 * GJB5369: 4.11.1.1
 * Inappropriate loop value type is forbidden
 */

void static_p(void) {
  float f = 0.0f;
  /* ... */

  /* The loop value type is inappropriate */
  for (f = 0.0f; f < 10.0f; f = f + 1.0f) {
    /* ... */
  }
}