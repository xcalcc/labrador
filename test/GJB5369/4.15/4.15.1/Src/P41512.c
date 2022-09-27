/*
 * GJB5369: 4.15.1.2
 * local variable name should be different from the global variable
 */

unsigned int Fire_Command;

void static_p(void) {
  /* conflict with the global value */
  unsigned int Fire_Command = 2u;
}