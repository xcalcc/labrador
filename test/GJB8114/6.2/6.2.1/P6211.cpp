/*
 * GJB8114: 6.2.1.1
 * Using global variables in construct function is forbidden
 */

int g = 10;

class A {
private:
  int a;

public:
  A(void) {
    a = g;  // imcompliance
  }
};