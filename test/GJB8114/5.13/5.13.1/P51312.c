/*
 * GJB8114: 5.13.1.2
 * Parameters' name conflicting with global variables is forbidden
 */

int sign = 0;
int func(int sign) {
  int local = 0;
  local = sign;
  return local;
}

int main(void) {
  return 0;
}