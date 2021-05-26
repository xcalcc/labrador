/*
 * GJB8114: 5.2.1.2
 * Loop body should be enclosed with brace
 */

int main() {
  for (int i = 0; i < 100; i++) i;

  int i = 3;
  while (i != 10) i = i + 1;

  do {
    i = i - 1;
  } while(i > 0);
}