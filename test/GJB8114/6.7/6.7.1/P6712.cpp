/*
 * GJB8114: 6.7.1.2
 * Returning non-const value from const member functions is forbidden
 */

class A {
private:
  int *a;

public:
  A() : a(nullptr) {}

  // imcompliance
  int *get() const {
    return a;
  }

  const int *get_c() const {
    return a;
  }
};