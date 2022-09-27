/*
 * GJB8114: 6.4.1.2
 * Overridden virtual functions in derived class should be noted with virtual
 */

class A {
private:
  int a;

public:
  virtual void set(int x) { a = x; }
};

class B : public A {
private:
  int b;

public:
  void set(int y) { b = y; }  // imcompliance
};

class C : public A {
private:
  int c;

public:
  virtual void set(int z) { c = z; }
};