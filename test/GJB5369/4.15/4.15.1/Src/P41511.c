/*
 * GJB5369: 4.15.1.1
 * enum name should be sole
 */

unsigned int duplicate = 0u;

void static_p(void) {
  /* conflict with global value */
  enum Name_type { e1, duplicate } EnumVar;
  EnumVar = e1;
}