#include <stdio.h>

void func (char *ptr);

void func (char *ptr) // pointer declared is not const-qualified
{
  // ptr pointed to is not modified, it should be const-qualified
  printf ("%c", *ptr);
}