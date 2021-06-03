/*
 * GJB8114: 5.6.1.2
 * Assigning overflowed value to integer is forbidden
 */

int main(void) {
  unsigned char data1 = 256;

  signed char data2 = -129;

  return 0;
}