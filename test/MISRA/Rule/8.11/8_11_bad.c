#include <stdio.h>

extern int array[]; // declare array with external linkage but without size
int foo( void );

int foo ( void )
{
  return array[10]; // 10 may be bigger than actual array size
}