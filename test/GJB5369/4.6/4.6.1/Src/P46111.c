/*
 * GJB5369: 4.6.1.11
 * logic non to const value is forbidden
 */

void static_p(void) {
  bool flag = false;
  
  /* logic non to const value is forbidden */
  if (flag == !1) {
    /* ... */
  }
}