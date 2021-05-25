/*
 * GJB8114: 5.1.2.5
 * Struct should not nest more than three levels
 */

struct A {
  struct B{
    struct C {
      int a;
    } x;
  };
};

int main(void) {
  struct A a;
  return 0;
}