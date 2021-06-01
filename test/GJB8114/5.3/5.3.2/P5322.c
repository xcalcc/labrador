/*
 * GJB8114: 5.3.2.2
 * Using void pointer carefully
 */

int x(void *a) {
  return 1;
}

int main(void) {
  void *a;
  int b = 3;
  a = &b;
  return 0;
}