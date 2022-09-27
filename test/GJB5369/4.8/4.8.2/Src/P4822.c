/*
 * GJB5369: 4.8.2.2
 * using ++ or -- should be carefully
 */

void foo(unsigned int p_x) {}

void static_p(void) {
  unsigned int x = 1u;
  unsigned int y = 2u;
  bool flag = false;

  if (flag == false) {
    x++;
    x = x + y++;    /* This form is not recommanded */
    foo(x++);
  }
}