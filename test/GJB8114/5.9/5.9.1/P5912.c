/*
 * GJB8114: 5.9.1.2
 * Condition variable of for-loop must be integer type
 */

int main(void) {
  float f = 0.0, g = 1.0;
  for (f = 0.0; f < 10.0; f = f + 1.0) {
    g = f + g;
  }
  return 0;
}