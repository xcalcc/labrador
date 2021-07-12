/*
 * GJB8114: 6.3.1.2
 * Exceptions in destruct function which are not catch by class it self should be forbidden
 */

class A {
private:
  int a;
public:
  A() : a(0) {}

  ~A() {
    throw 0;
  }
};