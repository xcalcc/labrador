#include <stdio.h>

extern int foo(int *a);
extern void foo2( void );
static int num = 0; // object declared, global scope

int count( void )
{
  num++; // object appears only in this function
  return num;
}

extern void foo2( void )
{
  foo(&num); // object may be modified at outer scope
}