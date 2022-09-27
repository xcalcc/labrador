/*
 * GJB8114: 5.10.1.1
 * Explicit cast is required when assigning float value to int variable
 */

int main(void) {
  int ix, iy;
  float fx = 1.85;
  float fy = -1.85;
  ix = fx;  // incompliance
  iy = fy;  // incompliance
  return 0;
}