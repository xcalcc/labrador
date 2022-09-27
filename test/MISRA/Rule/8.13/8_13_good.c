#include <stdio.h>

void func (const char *ptr);

void func (const char *ptr) // pointer declared const-qualified
{
  // ptr pointed to is not modified
  printf ("%c", *ptr);
}