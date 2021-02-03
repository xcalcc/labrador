/*
 * GJB5369: 4.1.2.9
 * using non-named bit fields carefully
 */

struct static_p {
  unsigned int x : 2, : 2;
};

void dummy(void) {}