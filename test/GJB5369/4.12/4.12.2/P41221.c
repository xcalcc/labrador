/*
 * GJB5369: 4.12.2.1
 * assign to pointer carefully
 */

void static_p(int i) {
  int *p_1;

  /* assigning non-pointer type without "&" 
   * to a pointer is forbidden  
   */
  p_1 = i;
}