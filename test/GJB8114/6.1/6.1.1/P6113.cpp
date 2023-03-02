/*
 * GJB8114: 6.1.1.3
 * "virtual" is needed when inheriting from base class in derivative design of diamond structure.
 */

#include <iostream>

using namespace std;

class A {
public:
  A(void) : a(0) {}

  void SetA(int b) {
    a = b;
  }

private:
  int a;
};

class B1 : public A { // imcompliance, need 'virtual'
private:
  int b1;
public:
  B1(void) : A(), b1(0) {}

  void SetB1(int b) {
    b1 = b;
  }
};

class B2 : public A { // imcompliance, need 'virtual'
private:
  int b2;

public:
  B2(void) : A(), b2(0) {}

  void SetB2(int a) {
    b2 = a;
  }
};

class D : public B1, public B2 {
private:
  int d;

public:
  D(void) : B1(), B2(), d(0) {}
};

int main(void) {
  D d;
  d.SetB1(1);
  d.SetB2(2);
  return 0;
}