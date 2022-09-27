/*
 * GJB8114: 5.7.1.11
 * void is required as the function which has return value is called but the return value is not used
 */

int f(int a) {
  return a;
}

int main(void) {
  int ret = f(2);
  (void)f(3);
  f(3);   // imcompliance
  return 0;
}