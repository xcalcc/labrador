/*
 * GJB5369: 4.10.2.1
 * avoid using comment without analysis
 */

void static_p(void) {
  unsigned int x = 0u;
  /* ... */
  x = 1u;
  /* Let x to be 1 */
  x = x + 1u;
  /* x becomes x add 1 */
}