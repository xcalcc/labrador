/*
 * GJB8114: 5.6.1.1
 * Assigning constant float to integer is forbidden
 */

int f() {
  int i;
  i = 3.3;
  return i;
}
