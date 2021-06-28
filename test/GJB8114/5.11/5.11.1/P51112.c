/*
 * GJB8114: 5.11.1.2
 * Omitting init value which depends on the system is forbidden
 */

int a;
int b = 0;
int main(void) {
  static int StateN;
  static int StateX = 0;
  if (a == 1) {
    StateN = StateN + 1;
    StateX = StateX + 1;
  }
  return 0;
}