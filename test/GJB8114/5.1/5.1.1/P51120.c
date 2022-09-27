/*
 * GJB8114: 5.1.1.20
 * char variable should be defined explicit with sign type
 */

int main(void) {
  char i;
  int j = i + 1;
  return 0;
}