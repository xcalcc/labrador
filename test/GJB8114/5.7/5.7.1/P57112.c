/*
 * GJB8114: 5.7.1.12
 * Void is not required as the function which is void type is called
 */

void func(int para) {
  int i = para + 1;
}

int main(void) {
  int local = 0;
  (void)func(local);  // imcompliance
  return 0;
}