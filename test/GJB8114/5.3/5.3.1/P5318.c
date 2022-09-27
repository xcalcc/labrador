/*
 * GJB8114: 5.3.1.8
 * Pointer should be checked between its first use and being alloced
 */

#include <malloc.h>

int main(void) {
  int *x = (int *) malloc(sizeof(int));
  *x = 1; // imcompliance: not check
  return 0;
}