/*
 * GJB5369: 4.8.1.1
 * avoid using "O" or "I" as variable names
 */

void static_p(void) {
    
  /* avoid using "O" or "I" as variable names */
  int I = 1, O = 0;
  I = O;
  O = I;
}