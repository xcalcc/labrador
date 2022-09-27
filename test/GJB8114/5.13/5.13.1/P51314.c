/*
 * GJB8114: 5.13.1.4
 * Variable names conflicting with identifiers in forbidden
 */

struct A {
  int x;
};

struct B {
  int y;
};

int main() {
  int A;  // incompliance
  int B;  // incompliance
}