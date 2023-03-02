/*
 * GJB8114: 6.6.1.1
 * Deleting the memory which allocated by new operator is a must
 */

void f(void) {
  int *p = new int; // imcompliance
  *p = 2;
}

int main(void) {
  f();
}