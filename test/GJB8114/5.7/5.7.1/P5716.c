/*
 * GJB8114: 5.7.1.6
 * Return type in function definition should keep up with declaration
 */

short fun(int a);
int fun(int a) {
  return 1;
}

int main(void) {
  int ret= fun(1);
  return 0;
}