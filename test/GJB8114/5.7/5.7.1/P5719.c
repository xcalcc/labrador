/*
 * GJB8114: 5.7.1.9
 * Return value is required in return statement if the function has return value
 */

int f(int a) {
  int c = a + 3;
}

int main(void) {
  int ret = f(2);
  return 0;
}