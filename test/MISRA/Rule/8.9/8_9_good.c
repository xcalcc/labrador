#include <stdio.h>

extern int foo (int *a);
extern void foo2 ( void );

int count ( void )
{
  static int num = 0; // object declared at block scope
  num++; // object appears only in this function
  return num;
}