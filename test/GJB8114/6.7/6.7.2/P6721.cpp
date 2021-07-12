/*
 * GJB8114: 6.7.2.1
 * Implement of member functions shouldn't in class definition
 */

class A {
private:
  int a;

public:
  A() : a(0) {}

  // imcompliance
  void set(int x) { a = x; }

  int get() const;
};

int A::get() const {
  return a;
}