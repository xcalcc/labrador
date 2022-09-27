/*
 * GJB8114: 5.7.2.3
 * Using abort/exit carefully
 */

int f1() {
  abort();
}

int main() {
  exit();
}