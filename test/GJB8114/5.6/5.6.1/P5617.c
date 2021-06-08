/*
 * GJB8114: 5.6.1.7
 * Using negative number as shifting number is forbidden
 */

int main(void) {
  int a = 232;
  a = a << -1;
  a = a >> -2;
  return 0;
}