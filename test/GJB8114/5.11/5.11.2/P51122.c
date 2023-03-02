/*
 * GJB8114: 5.11.2.1
 * All global variables should be initialized in a unified initialization module
 */

int g;
int h;

void init() {
  g = 1;
  h = 2;
}

int main() {
  init();
}