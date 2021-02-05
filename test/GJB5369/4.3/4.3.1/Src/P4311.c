/*
 * GJB5369: 4.3.1.1
 * non-statement is forbidden as the conditional
 * judgement is true:
 * 1. if (...) else
 * 2. if (...) {} else
 * 3. if (...) {;} else
 */

void static_p(void) {
  unsigned int value_x = 1u;

  if (value_x == 0u)
    ;

  if (value_x == 0u) {
#if FALSE
    value_x = value_x + 1u;
#endif
  }

  if (value_x == 0u) {
    ;
  }
}
