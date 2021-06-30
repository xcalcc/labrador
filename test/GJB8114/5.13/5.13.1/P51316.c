/*
 * GJB8114: 5.13.1.6
 * Variable names conflict with typedef value is forbidden
 */

typedef unsigned int TData;

int main(void) {
  int TData = 1;
  return 0;
}