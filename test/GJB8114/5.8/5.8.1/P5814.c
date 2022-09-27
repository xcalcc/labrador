/*
 * GJB8114: 5.8.1.4
 * Comment is required when using octal numbers
 */

int main(void) {
  int code[3];
  code[0] = 109;
  code[1] = 100;
  code[2] = 011;  // imcompliance
  return 0;
}