/*
 * GJB8114: 5.1.1.19
 * Using "extern" variable declaration with init statement is forbidden
 */

extern int a = 2;
int main(void) {
  a = 3;
  return 0;
}