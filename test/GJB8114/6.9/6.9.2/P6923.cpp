/*
 * GJB8114: 6.9.2.3
 * Overloading && or || is forbidden
 */

class A{
public:
  int x;
};

bool operator&&(A a, A b) {
  return a.x && b.x;
}

bool operator||(A a, A b) {
  return a.x || b.x;
}