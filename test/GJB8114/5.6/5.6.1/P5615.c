/*
 * GJB8114: 5.6.1.5
 * Using ++ or -- in arithmetic statement or function parameters is forbidden
 */

int f(int i) { return i; }

int main(void) {
  int x = 1, y = 2, z = 3;
  y = y + (x++);
  z = z + (++y);
  x = f(++z);
  z = f(x++);
  return z;
}