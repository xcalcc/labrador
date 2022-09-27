/*
 * GJB8114: 6.7.1.1
 * Using reference to pass a array whose size is constant
 */

// imcompliance
int f1(int p[10]) {
  return p[0];
}

int f2(int *p) {
  return p[0];
}

int f3(int (&p)[10]) {
  return p[9];
}

int main(void) {
  int p[10];
  f1(p);
  f2(p);
  f3(p);
}