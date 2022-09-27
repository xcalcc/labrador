/*
 * GJB8114: 5.4.1.1
 * else statement is a must within if-else statement
 */

int main(void) {
  int i = 2;
  if (i > 3) {
    i = 5;
  }
  return 0;
}