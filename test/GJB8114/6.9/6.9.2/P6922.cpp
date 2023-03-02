/*
 * GJB8114: 6.9.2.2
 * It is recommended that const be in the outermost layer of the description except for the constant pointer
 */

int main(void) {
  const int a = 0;
  int const b = 3;  // imcompliance
}