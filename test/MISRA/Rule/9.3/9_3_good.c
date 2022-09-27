#include <stdio.h>

int func( void );

int func ( void )
{
  int a[10] = {0}; // array totally initialized
  return a[9]; // a[9] is '0'
}