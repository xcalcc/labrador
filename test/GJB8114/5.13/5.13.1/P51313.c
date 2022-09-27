/*
 * GJB8114: 5.13.1.3
 * Variable names conflicting with function names is forbidden
 */

void a() {
  return;
}

int main(void) {
  int a = 2;
  return 0;
}