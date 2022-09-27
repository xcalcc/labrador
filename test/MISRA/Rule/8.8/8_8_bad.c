#include <stdio.h>

static int func (int a); // function with internal linkage declared

int func (int a) // function defined without "static"
{
  return a;
}