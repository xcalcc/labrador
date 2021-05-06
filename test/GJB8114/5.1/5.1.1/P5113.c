/*
 * GJB5111: 5.1.1.3
 * Self-defined types(typedef) redefined as other types is forbidden
 */

typedef int mytype;
int main() {
  typedef float mytype;
  mytype x = 1;
  return 0;
}