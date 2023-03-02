/*
 * GJB8114: 6.2.1.3
 * Construct functions which contains only one parameter should be note by "explicit"
 */

class A {
private:
  int a;
public:
  A(int b) : a(b) {}  // imcompliance
};

class B {
private:
  int b;
public:
  explicit B(int a) : b(a) {}
};