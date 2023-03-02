/*
 * GJB8114: 5.3.1.2
 * & is a must as using function as a pointer
 */

int func(int p1, int p2);

int main(void) {
  int a = 2;
  if (func != 0) {
    a = 3;
  }
  return 0;
}