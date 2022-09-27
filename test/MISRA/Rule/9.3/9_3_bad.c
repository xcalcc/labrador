#include <stdio.h>

int func( void );

int func ( void )
{
  int a[10] = {0, 0, 0}; // array only initialized three elements
  return a[9]; // a[9] is still unknown value
}