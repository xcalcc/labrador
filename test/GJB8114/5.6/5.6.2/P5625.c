/*
 * GJB8114: 5.6.2.5
 * Avoid using strcat function
 */

#include <string.h>

int main(void) {
  const char *a = "hello world";
  char des[10]={2};
  strcat(des, a);
}