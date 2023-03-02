#include <stdio.h>

static int func (int a); // function with internal linkage declared

static int func (int a) // function defined with "static"
{
  return a;
}