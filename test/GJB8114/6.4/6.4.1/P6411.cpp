/*
 * GJB8114: 6.4.1.1
 * Default parameters in virtual function of base shouldn't be changed by derived classes
 */

class A {
private:
  int a;

public:
  virtual void set(int x = 1) { a = x; }
};

class B : public A {
private:
  int b;

public:
  void set(int x = 0) { b = 2; }  // imcompliance
};

class C : public A {
private:
  int c;

public:
  void set(int x = 1) { c = x; }
};