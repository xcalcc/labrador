#include <stdio.h>

extern int foo (const char *a);
extern int foo2 (const char *b);

int func()
{
  const char *str = "string"; // str is not const-qualified
  foo ("string");
  foo2 ("string"); // string can not be modified
  return 0;
}