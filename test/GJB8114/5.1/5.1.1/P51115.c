/*
 * GJB8114: 5.1.1.15
 * Parameters should be declared with type
 */

int func(height) {
  int h;
  h = height + 10;
  return h;
}

int main(void) {
  func(1);
  return 0;
}