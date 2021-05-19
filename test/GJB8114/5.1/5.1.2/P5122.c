/*
 * GJB8114: 5.1.2.2
 * Ensure the brace is used pairedly is macro
 */

#define IF0(x) if(0 == (x)) {

int main(void) {
  int test = 0;
  IF0(test)
    test = test + 1;
  }
}