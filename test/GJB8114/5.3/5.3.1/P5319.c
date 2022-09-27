/*
 * GJB8114: 5.3.1.9
 * Using NULL to stand a nullptr instead of using 0
 */

#include <stdlib.h>
#include <malloc.h>

int main(void) {
  int *x = (int *)malloc(sizeof(int));
  if (x != 0) { // not incompliance
    *x = 1;
  }
  x = 0; // not incompliance
}