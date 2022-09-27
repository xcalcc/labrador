/*
 * GJB5369: 4.11.2.3
 * avoid using uncontrollable loop value
 */

#define defval 0

void static_p(void) {
  /* avoid using uncontrollable loop value */
  while (0) {
  }
  /* avoid using uncontrollable loop value */
  while (defval) {
  }
}