// non-POD struct
struct A {
  int a;
  int b[];
  ~A(){}
};

struct X;

// POD non-compliant
struct B {
  int b;
  char c;
};

// normal class
class C {
  int c;
public:
  int getC() { return c; }
};