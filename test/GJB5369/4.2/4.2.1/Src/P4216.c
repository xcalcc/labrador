/*
 * GJB5369: 4.2.1.6
 * the macro parameters should be enclosed in parentheses
 */

/* the macro parameters (x) should be enclosed in parentheses */
#define static_p(x) x >= 0 ? x : -x

void test_p(void) {
  unsigned int result;
  int a = 6, b = 5;
  result = static_p(a - b);
  result = static_p(a) + 1;
}