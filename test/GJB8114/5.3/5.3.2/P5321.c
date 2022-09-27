/*
 * GJB8114: 5.3.2.1
 * Using function pointer carefully
 */

void f(void) {}

int main(void) {
  void  (*a)(void) = f;
  a();
  return 0;
}