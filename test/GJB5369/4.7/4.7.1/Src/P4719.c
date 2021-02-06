/*
 * GJB5369: 4.7.1.9
 * formal and real parameters' type should be the same
 */

void foo(unsigned short p_1, unsigned short p_2) { /* ... */
}

void static_p(unsigned int p_1, unsigned short p_2) {
  foo(p_1, p_2);
  /* ... */
}