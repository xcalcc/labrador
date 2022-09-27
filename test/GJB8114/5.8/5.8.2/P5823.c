/*
 * GJB8114: 5.8.2.3
 * Avoid using useless function
 */

// 1. it doesn't have side effect
// 2. it doesn't have return value
void f(int a) {
  int b = 3;
}

int main(void) {
  int a = 4;
  f(a);
}