/*
 * GJB8114: 5.12.1.3
 * Comparing floats is forbidden
 */

int main(void) {
  int i, j;
  int p = 1000;
  float d = 0.435;
  if (435 == (p * d)) {
    i = 1;
  } else {
    i = 2;
  }
  if (435 != (p * d)) {
    j = 1;
  } else {
    j = 2;
  }
  return 0;
}