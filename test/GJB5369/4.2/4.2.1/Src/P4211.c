/*
 * GJB5369: 4.2.1.1
 * procedure must be enclosed in braces
 */

int f(int n = 2) try {
  ++n; // increments the function parameter
  throw n;
} catch(...) {
  ++n; // n is in scope and still refers to the function parameter
  assert(n == 4);
  return n;
}