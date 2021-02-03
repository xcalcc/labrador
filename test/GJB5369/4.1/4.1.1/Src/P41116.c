/*
 * GJB5369: 4.1.1.6
 * without the parameter declarations in the
 * function declaration is forbidden
 */

int static_p(); /* no param declarations */
int static_p(unsigned int p) {
  int x = 1;
  if (p == 0) {
    x = 0;
  }
  return x;
}