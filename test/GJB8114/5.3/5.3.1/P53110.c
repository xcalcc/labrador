/*
 * GJB8114: 5.3.1.10
 * File descriptor should be closed before program exit
 */

#include <stdio.h>

int main(void) {
  int fs = fopen("data", "r");
  return 0;
}