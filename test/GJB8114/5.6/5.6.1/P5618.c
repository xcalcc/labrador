/*
 * GJB8114: 5.6.1.8
 * Accessing array out of boundary is forbidden
 */

void f(void) {
  int a[100];
  for (int i = 0; i < 10; i++) {
    a[i] = 10;  // incompliance
  }
}