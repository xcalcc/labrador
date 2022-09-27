/*
 * GJB8114: 5.1.1.7
 * the macro parameters should be enclosed in parentheses
 */

#define pabs(x) x >= 0 ? x : -x

int main(void) {
  int a = 7, res;
  res = pabs(a) + 1;
  return 0;
}