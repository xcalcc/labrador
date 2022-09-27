/*
 * GJB5369: 4.6.1.7
 * bits can only be defined as signed/unsigned int type
 */

struct static_p {
  unsigned char x : 1; /* should be signed/unsigned type */
};
void dummy(void) {}