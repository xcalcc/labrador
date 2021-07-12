/*
 * GJB8114: 6.5.1.2
 * Removing const or volatile from type conversion of pointer or reference
 */

class A {
public:
  int a;

  A(int x) : a(x) {}
};

int main(void) {
  A const a1 = A(10);
  A *a2 = const_cast<A *>(&a1);
  a2->a = 11;
  A &a3 = const_cast<A &>(a1);
  a3.a = 12;
  return 0;
}