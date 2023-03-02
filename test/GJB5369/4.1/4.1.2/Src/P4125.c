/*
 * GJB5369: 4.1.2.5
 * using "#define" in functions is forbidden
 */

void static_p(unsigned int p_1) {
  unsigned int local_x = 0u;

#define BLOCKDEF 1u /* using "#define" in a function is forbidden */
  local_x = local_x + BLOCKDEF;
}