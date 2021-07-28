foo(void) {
  return 1;
}

extern x;

int main(void) {
  long long int c = foo();
  return 0;
}