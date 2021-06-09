/*
 * GJB8114: 5.6.1.14
 * Reading from buffer out of boundary is forbidden
 */

#include<string.h>

int main(void) {
  int src[2] = {0};
  int des[4];
  memcpy(des, src, sizeof(des));
  return (0)
}