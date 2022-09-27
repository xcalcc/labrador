/*
 * GJB5369: 4.8.2.4
 * Binocular operation should be using carefully
 */

void foo(unsigned int p_1) { /* ... */ }

void static_p(unsigned int p_1) {
  static unsigned int type0 = 0u;
  static unsigned int type1 = 1u;

  /* Binocular operation should be using carefully */
  (p_1 == 0) ? foo(type0) : foo(type1);
}