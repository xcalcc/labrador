/*
 * GJB8114: 6.2.1.4
 * All class members should be initialized in contruct function
 */


class A {
private:
  int a;
public:

  A(void) : a(0) {}

  explicit A(int x) {
    a = x;
  }
};

class B {
private:
  int b;

public:
  B(void) {}

  explicit B(int a) {}
};