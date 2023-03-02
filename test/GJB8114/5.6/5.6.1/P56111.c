/*
 * GJB8114: 5.6.1.11
 * Enum value used by non-enum variable is forbidden
 */

int main(void) {
  enum Edata {
    A = 0, B, C
  };
  unsigned int data;
  data = B;

  return 0;
}