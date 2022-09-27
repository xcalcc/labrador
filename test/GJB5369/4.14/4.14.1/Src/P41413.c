/*
 * GJB5369: 4.14.1.3
 * logical expression is forbidden in switch statement
 */

void static_p(void) {
  bool flag = false;
  switch (flag) {
  case true:
    break;
  case false:
    break;
  default:
    break;
  }
}