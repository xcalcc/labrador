/*
 * GJB8114: 5.3.1.5
 * Using freed memory is forbidden
 */

#include <malloc.h>

int f(void) {
  int *x = (int *)malloc(sizeof(int));
  free(x);
  free(x);
}

int main(void) {
  int *x = NULL;
  free(x);  // incompliance
}