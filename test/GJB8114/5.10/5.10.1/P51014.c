/*
 * GJB8114: 5.10.1.4
 * Type of value which assigned to pointer variables should keep up with the pointer type
 */

int main(void) {
  unsigned int *ptr = 0;
  unsigned short uid = 0;
  unsigned int a = 0;
  ptr = (unsigned short *)(&uid); // incompliance
  ptr = (&uid); // incompliance

  ptr = &a;
}