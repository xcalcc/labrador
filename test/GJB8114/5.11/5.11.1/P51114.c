/*
 * GJB8114: 5.11.1.4
 * Initial value of enum elements should be complete
 */
int main(void) {
  enum Etype {
    RED, WHITE = 0,
    BLUE
  } edata;
  edata = BLUE;
  return 0;
}