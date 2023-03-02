/*
 * GJB5369: 4.13.1.1
 * inital value is a must for the enum
 */

void static_p(void) {
  /* num1 without inital value */
  enum E_type { num1, num2 = 2, num3 = 3 };
}