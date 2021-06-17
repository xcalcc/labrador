/*
 * GJB8114: 5.8.1.2
 * Unreached branch is forbidden
 */

int main(void) {
  int i = 3;
  if (i > 0) {
    return 1;
  } else {  // unreachable
    return 0;
  }
}