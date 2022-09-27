/*
 * GJB8114: 5.6.1.9
 * Index of array must be non-negative integer
 */

int main(void) {
  int data[3] = {0, 0, 0};
  int i = -1;
  data[i] = 1;
  return 0;
}