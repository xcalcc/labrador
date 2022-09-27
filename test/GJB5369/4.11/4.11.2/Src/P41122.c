/*
 * GJB5369: 4.11.2.2
 * avoid using break in a loop
 */

void static_p(void) {
  int i = 10;
  while (i > -1) {
    if (i == 0) {
      
      /* avoid using break in a loop */
      break;
    }
    i = i - 1;
  }
}