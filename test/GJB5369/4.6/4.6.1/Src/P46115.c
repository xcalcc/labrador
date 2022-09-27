/*
 * GJB5369: 4.6.1.15
 * '=' used in logical expression is forbidden
 */

void static_p(void) {
  bool flag = false;
 
  /* This condition should be (flag == false) */
  if (flag = false) {
  }
}