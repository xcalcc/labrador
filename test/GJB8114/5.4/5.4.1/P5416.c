/*
 * GJB8114: 5.4.1.6
 * Switch statement should contain default
 */
enum X {
  A, B, C, D
};

int main(void) {
  enum X i = A;
  switch (i) {
    case A:
      return 0;
    case B:
      return 1;
      // need default here
  }
}