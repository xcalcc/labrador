/*
 * GJB8114: 5.13.1.15
 * Assigning negative value to unsigend variables is forbidden
 */

int main(void) {
  unsigned short a;
  a = -10;
}