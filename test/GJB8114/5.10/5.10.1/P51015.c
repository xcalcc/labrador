/*
 * GJB8114: 5.10.1.5
 * Explicit cast is required by assignments between pointer type value and non-pointer type value
 */

int main(void) {
  unsigned int *ptr = 0;
  unsigned int adr = 0;
  unsigned int uid = 0;

  ptr = adr;  // incompliance
  adr = &uid; // incompliance

  ptr = (unsigned int *)adr;
  adr = (unsigned int)(&uid);
  return 0;
}