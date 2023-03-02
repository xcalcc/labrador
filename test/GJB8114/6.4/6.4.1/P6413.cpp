/*
 * GJB8114: 6.4.1.3
 * Non-pure virtual function being overridden as pure virtual function is forbidden
 */

class A {
private:
  int a;
public:
  A() : a(0) {}

  virtual void set(int x) { a = x; };

};

class B : public A {
private:
  int b;
public:
  B() : A(), b(0) {}

  virtual void set(int x) = 0;
};