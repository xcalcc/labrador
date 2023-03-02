/*
 * GJB8114: 6.6.1.3
 * The pointer which will be deleted should points to the origin address which allocated by new
 */

int main(void) {
  int *p = new int[3];
  *p = 1;
  p++;
  *p = 2;
  delete[] p;
}