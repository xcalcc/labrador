/*
 * GJB8114: 6.2.1.5
 * Derived class should contain constructor of base class
 */

class A {
private:
  int a;
public:
  A() : a(0) {}
};

class B : public A {
private:
  int b;
public:
  B() : A(), b(0) {}
};

class C : public A {
private:
  int c;
public:
  C() : c(0) {} // imcompliance
};