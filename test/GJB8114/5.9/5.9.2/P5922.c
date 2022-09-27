/*
 * GJB8114: 5.9.2.2
 * Using multiple break statements in loop should be careful
 */

int main(void) {
  for (int i = 0; i < 5; i++) {
    break;
    break;
  }
}