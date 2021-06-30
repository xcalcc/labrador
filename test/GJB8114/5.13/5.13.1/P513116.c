/*
 * GJB8114: 5.13.1.16
 * String array must be end with '\0'
 */

int main(void) {
  char buf[3];
  buf[0] = '1';
  buf[1] = '2';
  buf[2] = '3'; // incompliance
}