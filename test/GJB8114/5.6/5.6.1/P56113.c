/*
 * GJB8114: 5.6.1.13
 * Assignment in sizeof() is forbidden
 */

int main(void) {
  int x = 1, y = 2;
  int ilen;
  ilen = sizeof(x=y);
  return ilen;
}