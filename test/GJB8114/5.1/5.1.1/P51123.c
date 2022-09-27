/*
 * GJB8114: 5.1.1.23
 * Using void as the parameters list is empty
 */

int fun(){
  return 3;
}

int main(void) {
  int a = fun();
  return 0;
}