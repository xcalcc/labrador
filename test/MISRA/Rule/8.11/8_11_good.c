#include <stdio.h>

extern int array[20]; // declare array with external linkage and its size
int foo ( void ) ;

int foo ( void )
{
  return array[10];
}