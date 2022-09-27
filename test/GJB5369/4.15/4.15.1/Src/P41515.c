/*
 * GJB5369: 4.15.1.5
 * redefining the exist variable is frobidden
 */

void static_p(unsigned int p_1) {
  unsigned int static_p;
  bool c_1 = false;
  if (c_1) {
    /* redefining static_p is forbidden */
    unsigned int static_p = 1u;
    static_p = static_p + 1u;
  } else {
    static_p = p_1;
  }
}