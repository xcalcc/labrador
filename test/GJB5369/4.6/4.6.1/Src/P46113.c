/*
 * GJB5369: 4.6.1.13
 *  using enumeration types beyond the limit if forbidden
 */

void static_p(void) {
  enum Etype { Enum1, Enum2, Enum3 };
  unsigned int ui;

  /* enum can only compare to the same enum type */
  ui = Enum1;
}