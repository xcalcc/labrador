/*
 * GJB8114: 5.6.1.18
 * Using gets function is forbidden
 */

#include <stdio.h>
int main(void) {
  char line[5] = {0};
  gets(line);
  return 0;
}