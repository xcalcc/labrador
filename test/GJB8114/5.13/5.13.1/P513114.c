/*
 * GJB8114: 5.13.1.14
 * Using NULL as zero is forbidden
 */

#include <stdlib.h>

int fun(int width) {
  return width + 1;
}
int main(void) {
  int a = fun(NULL);
  return 0;
}