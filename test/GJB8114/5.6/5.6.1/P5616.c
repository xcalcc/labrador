/*
 * GJB8114: 5.6.1.5
 * Shifting the variable beyond its length is forbidden
 */

int main(void) {
  unsigned int x, y, z;
  x = 0x0000001;
  y = x << 33;
  x = 0x80000000;
  z = x >> 33;
}