/*
 * GJB8114: 5.4.1.4
 * Using bool with switch statement is forbidden
 */

int main(void) {
  int i = 3;
  switch (i == 2) {
    case 1:
      return 1;
    default:
      return 0;
  }
}