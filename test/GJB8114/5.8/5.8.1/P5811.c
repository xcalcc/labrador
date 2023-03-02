/*
 * GJB8114: 5.8.1.1
 * Dead code is forbidden
 */

int f(int i) {
  switch (i) {
    int a = 199;  // dead code
    case 1:
      return 1;
    case 2:
      return 2;
    default:
      return 3;
  }
}