#include <stdio.h>

int func(void);

int func(void) {
  // arr have designated initializer with size specified explicitly
  int arr[10] = {[5] = 1};
  return arr[9];
}