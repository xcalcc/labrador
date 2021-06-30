/*
 * GJB8114: 5.13.1.5
 * Variable names conflicting with enum elements is forbidden
 */

enum Name_type {
  first = 0, secode
} EnumVar;

int main(void) {
  unsigned int second = 0;
  EnumVar = second;
  return 0;
}