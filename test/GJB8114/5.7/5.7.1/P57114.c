/*
 * GJB8114: 5.7.1.14
 * Multiple function call is one statement is forbidden
 */

int f1(int *p) {
  *p = 1;
  return *p;
}

int f2(int *p) {
  *p = 2;
  return *p;
}

int f3(int *p) {
  *p = 3;
  return *p;
}

int main(void) {
  int a =3;
  a = f1(&a) + f2(&a);
  a = f3(&a);
  return 0;
}