/*
 * GJB8114: 5.6.1.17
 * The pointer being freed should points to the address which alloced by malloc
 */

#include <stdlib.h>
#include <malloc.h>

int fun(void) {
  int *p = (int *)malloc(3*sizeof(int));
  if (NULL == p) {
    return (-1);
  } else {
    *p = 1;
    p++;
    *p = 2;
    free(p);
    p = NULL;
  }
  return 0;
}

int main() {
  int i = fun();
  return 0;
}