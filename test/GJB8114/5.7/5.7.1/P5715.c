/*
 * GJB8114: 5.7.1.5
 * Parameters' type in function declaration should keep up with definition
 */

int func(int d1, int d2);

int func(short d1, short d2) {

}

int main(void) {
  int ret = func(1, 2);
  return 0;
}