/*
 * GJB8114: 5.11.1.1
 * Using uninitialized variable is forbidden
 */

int main(void) {
  int i;
  float x, y, z;
  x = z;        // incompliance
  if (i == 0) { // incompliance
    y = z;      // incompliance
  }
  return 0;
}