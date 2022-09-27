/*
 * GJB8114: 6.6.1.2
 * Memory allocated by new[] should be deleted by delete[]
 */

int main(void) {
  int *p = new int[3];
  p[0] = 1;
  p[1] = 2;
  p[2] = 3;
  delete p; // imcompliance
  return 0;
}