/*
 * GJB8114: 5.6.2.4
 * Avoid using strcpy function
 */

#include <string.h>

int main() {
  char a[20] = {3};
  strcpy(a, "ddd");
  return 0;
}