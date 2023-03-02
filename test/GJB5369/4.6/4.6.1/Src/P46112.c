/*
 * GJB5369: 4.6.1.12
 * bit-wise operation to signed-int is forbidden
 */

void static_p(void) {
  int b = 1;
  
  /* bit-wise operation to signed-int is forbidden */
  b = b | 1;    
}