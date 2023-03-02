/*
 * GJB8114: 5.1.1.17
 * Using "extern" variable in function is forbidden
 */

int main(void) {
  extern int zdata;
  extern int fadd(int, int);
  zdata = fadd(1, 2);
  return 0;
}