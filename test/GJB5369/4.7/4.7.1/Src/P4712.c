/*
 * GJB5369: 4.7.1.2
 * dead function should be avoided
 */

/* The function will never been call which should be deleted. */
unsigned int static_p(unsigned int p) {
  unsigned int x = 1u;
  x = x + p;
  return x;
}

int main(void) { return (0); }