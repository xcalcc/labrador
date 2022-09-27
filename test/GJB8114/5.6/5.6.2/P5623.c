/*
 * GJB8114: 5.6.2.3
 * Free the memory which is dynamic applied in time
 */

#include <malloc.h>

int main(void) {
  int *a = (int *)malloc(4);
  *a = 0;
  return 0;
}