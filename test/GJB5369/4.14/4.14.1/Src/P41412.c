/*
 * GJB5369: 4.14.1.2
 * Logical discriminant can only be logical expression
 */

void static_p(void) {
  unsigned int x = 0u;
  if (x) {
    /* ... */
  }
}