#include <stdio.h>

extern int foo (char *a);
extern int foo2 (char *b);

int func()
{
  char *str = "string"; // str is not const-qualified
  foo ("string");
  foo2 ("string"); // may be modified by foo
  return 0;
}