/*
 * GJB8114: 5.1.1.1
 * Changing the definition of basic type or keywords by macro is forbidden
 */

#define long 100  // incompliment

#define LONG_NUM 100 // compliment

int main(void) {
  int i;
  i = long;
  return 0;
}
