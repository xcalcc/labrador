/*
 * GJB8114: 5.1.1.6
 * Using #define and #undef pairedly
 */
unsigned int fun1(void);
unsigned int fun2(void);

int main(void) {
  unsigned int res = 0;
  res = fun1();
  res += fun2();
  return 0;
}

unsigned int fun1(void) {
  unsigned int x = 0U;
#define BLOCKDEF 2U
  x = x + BLOCKDEF;
  return x;
}

unsigned int fun2(void) {
  unsigned int x = 0U;
  x = x + BLOCKDEF;
#undef BLOCKDEF
  return x;
}