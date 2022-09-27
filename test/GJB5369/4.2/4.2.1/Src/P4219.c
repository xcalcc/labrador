/*
 * GJB5369: 4.2.1.9
 * '\' used alone in a string is forbidden
 */

void static_p(void) {
  unsigned char *str = (unsigned char *)"string\
    literal";
}