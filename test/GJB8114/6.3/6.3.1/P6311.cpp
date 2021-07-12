/*
 * GJB8114: 6.3.1.1
 * Destruct function of classes which contain the virtual functions should be virtual
 */

class A {
private:
  int a;

public:
  A(void) : a(0) {}
  virtual int get(void) const { return a; }
  ~A(void) {} // imcompliance
};

class B {
private:
  int b;

public:
  B(void) : b(0) {}
  virtual int get(void) const { return b; }
  virtual ~B(void) {} // compliance
};