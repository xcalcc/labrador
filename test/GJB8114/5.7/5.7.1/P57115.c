/*
 * GJB8114: 5.7.1.15
 * Using … in function parameter list is forbidden
 */

int fun(int data, ...) {
  int temp = 2*data;
  return temp;
}
