/*
 * GJB8114: 5.4.1.5
 * Switch statement contains only default is forbidden
 */

int main(void) {
  int i = 2;
  switch (i) {
    default:
      return i;
  }
}