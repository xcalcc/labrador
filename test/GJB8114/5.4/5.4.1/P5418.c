/*
 * GJB8114: 5.4.1.8
 * Cases of switch should have the same hierarchy range
 */

int main(void) {
  int i = 2;
  switch (i) {
    case 1: {
      if (i == 2) {
        case 2: return 1;
      }
      break;
    }
    return 0;
  }
}