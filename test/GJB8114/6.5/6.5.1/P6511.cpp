/*
 * GJB8114: 6.5.1.1
 * Converting unrelated pointers to object pointers is forbidden
 */

struct S {
  int i;
};

class A {
private:
  int a, b;
public:
  A(void) : a(0), b(0) {}
};

int main(void) {
  struct S s1;
  A *a = reinterpret_cast<A *>(&s1);
  return 0;
}