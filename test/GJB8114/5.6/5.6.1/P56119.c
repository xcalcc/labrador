/*
 * GJB8114: 5.6.1.19
 * Be careful with the boundary of strings when operating on it
 */
#include <string.h>
int main(void) {
  char string1[10] = {0};
  char string2[10] = {0};
  strncpy(string1, "hello world", 11);
  strncpy(string2, "hello", 6);
  strncat(string2, "world", 5);
  return 0;
}