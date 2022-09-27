/*
 * GJB8114: 5.4.2.1
 * Branches nested more than 7 level is forbidden
 */

int f(int i) {
  if (i >10) {
    if (i > 9) {
      if (i > 8) {
        if (i > 7) {
          if (i > 6) {
            if (i > 5) {
              if (i > 4) {
                return 1;
              }
            }
          }
        }
      }
    }
  }
  return 2;
}