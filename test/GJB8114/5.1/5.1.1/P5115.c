#define BLOCKDEF 1
int main() {
#define BLOCKDEF 2
  int a = BLOCKDEF;
  int b = 2;
  b = a + b + BLOCKDEF;
}