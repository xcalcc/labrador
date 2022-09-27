/*
 * GJB8114: 5.6.1.15
 * Write to buffer out of boundary is forbidden
 */

#include <string.h>

int f() {
  int src[4]={1};
  int des[2];
  memcpy(des, src, sizeof(src));
}