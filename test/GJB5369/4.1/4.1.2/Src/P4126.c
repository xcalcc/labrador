/*
 * GJB5369: 4.1.2.6
 * using "#undef" in functions is forbidden
 */

#define BLOCKDEF 1u

void static_p(void) {
  unsigned int local_x = 0u;
  local_x = local_x + BLOCKDEF;
#undef BLOCKdEF;    /* using "#undef" in functions is forbidden. */
}