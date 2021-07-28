#include <stdio.h>

void func ( void );

void func ( void )
{
  short int short_a, short_b;
  long int long_a, long_b;
  long_a = 1;
  short_a = short_b = 2;
  long_b = ( ( long ) short_a * short_b ) * long_a; // explicitly converted to long
}