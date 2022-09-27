/*
 * GJB8114: 5.5.1.1
 * using "goto" to get in compound statements is forbidden
 */

int f(int i) {
  L1:
  i = i + 1;
  if (i == 3) {
    goto L1;
  }

  if (i == 4) {
    goto L2;
  } else {
    L2:
    return 2;
  }
}