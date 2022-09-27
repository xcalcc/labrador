/*
 * GJB5369: 4.3.1.3
 * else block should not be empty
 * 1. else;
 * 2. else {}
 * 3. else {;}
 */

void static_p(void) {
  unsigned int name_x = 1u;

  /* else block should not be empty */
  if (name_x == 0u) {
  } else;

  if (name_x == 0u) {
  } else {

#if FALSE
    name_x = name_x + 1u;
#endif

  }

  if (name_x == 0u) {
  } else {
    ;
  }
}
