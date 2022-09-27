/*
 * GJB5369: 4.1.2.4
 * using bit-field in struct should be carefully
 */

struct bitfield1 {
  unsigned int x : 1;   /* using bit-field should be carefully */
};

void dummy(void) {}