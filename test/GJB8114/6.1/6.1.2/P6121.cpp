/*
 * GJB8114: 6.1.2.1
 * Deriving from virtual base class should be carefully
 */

class Base {
private:
  int a;

public:
  Base() : a(0) {}

  virtual ~Base() {}
};

class A : virtual public Base { // carefully here
private:
  int b;

public:
  A() : Base(), b(0) {}

  ~A() {}
};