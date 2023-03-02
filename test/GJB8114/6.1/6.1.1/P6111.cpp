/*
 * GJB8114: 6.1.1.1
 * Copy construct function is a must for classes which has dynamic allocated memory members
 */

#include <iostream>

using namespace std;

class A {
private:
  char *str;
public:
  A(char *cstr) {
    str = new char[20];
    strncpy(str, cstr, 20);
  }

  A(const A &ca) {
    str = new char[20];
    strncpy(str, ca.str, 20);
  }

  A operator=(const A &ca) {
    if (NULL == str) {
      str = new char[20];
    }
    strncpy(str, ca.str, 20);
  }

  ~A() {
    delete[] str;
    str = NULL;
  }
};