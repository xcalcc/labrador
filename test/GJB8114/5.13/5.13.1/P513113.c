/*
 * GJB8114: 5.13.1.13
 * Multiple then same volatile variables existing in one statement is forbidden
 */

int main(void) {
  int i= 1;
  volatile int v = 2;
  int x = v + v + i;  // incompliance
  return 0;
}