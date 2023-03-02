/*
 * GJB5369: 4.6.1.6
 * signed-value must be longer than two bits
 */

struct static_p {
  int x : 1;
};

void dummy(void) {}