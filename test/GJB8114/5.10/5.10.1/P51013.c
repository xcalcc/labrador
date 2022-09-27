/*
 * GJB8114: 5.10.1.3
 * Explicit cast is required when assigning double value to float variable
 */

int main(void) {
  double dData = 0.0;
  float fData;
  fData = dData;  // incompliance
  fData = (float) dData;
}