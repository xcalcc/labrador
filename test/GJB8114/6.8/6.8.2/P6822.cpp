/*
 * GJB8114: 6.8.2.2
 * Throwing pointer carefully
 */

int main(void) {
  int i;
  try {
    throw &i; // imcompliance
  } catch (int *) {

  }
}