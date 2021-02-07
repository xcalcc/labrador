/*
 * GJB5369: 4.14.2.1
 * avoid using complex logical expression
 */

#define ISTRAC 2
#define IT 3

void static_p(void) {
  int trcsta = 0, fdiret = 1;

  /* Too complex, should be writen:
   * if((trcsta > ISTRAC)&&(fdiret>IT))
   */
  if ((!(trcsta <= ISTRAC)) && (!(fdiret <= IT))) {
      /* ... */
  }
}