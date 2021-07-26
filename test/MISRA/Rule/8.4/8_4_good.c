#include <stdio.h>

extern int func(int a); // extern function declared

int func(int a) // extern compatible funtion defined
{
  return 0;
}