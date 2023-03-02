/*
 * GJB8114: 5.10.1.6
 * Using meaningless cast is forbidden
 */

int main(void) {
  int a = 10;
  int b = 2;
  a = (int) b;
}