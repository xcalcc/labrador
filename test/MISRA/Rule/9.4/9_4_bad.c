
void func(void);

void func(void) {
  // a[0] initialized twice
  int arr[3] = {[0] = 3, [1] = 2, [2] = 1, [0] = 4};
}