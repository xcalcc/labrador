/*
 * GJB8114: 5.6.1.16
 * Using freed memory is forbidden
 */

#include <stdlib.h>
#include <malloc.h>

int main(void) {
  int *x = (int *) malloc(sizeof(int));
  int y;
  if (x != NULL) {
    *x = 1;
    free(x);
    x = NULL;
  } else {
    return -1;
  }
  y = (*x);
  return 0;
}