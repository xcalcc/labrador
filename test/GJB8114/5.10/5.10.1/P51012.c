/*
 * GJB8114: 5.10.1.2
 * Explicit cast is required when assigning int value to shorter int variable
 */

int main(void) {
  signed char var = 0;
  short svar = 0;
  int ivar = 0;
  long lvar = 0;
  var = svar;  // incompliance
  svar = ivar; // incompliance
  ivar = lvar; // incompliance
}