#include <stdio.h>

int func(void);

int func(void) {
  // flexible array declared using designated initializer
  int arr[] = {[5] = 1};
  // array may not have the element arr[9]
  return arr[9];
}