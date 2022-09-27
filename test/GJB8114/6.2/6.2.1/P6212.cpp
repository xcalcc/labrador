/*
 * GJB8114: 6.2.1.2
 * Default construct functions should be defined explicitly in class
 */

// imcompliance
class A {
public:
  ~A() {}
};

class B {
public:
  B(void) {}
};