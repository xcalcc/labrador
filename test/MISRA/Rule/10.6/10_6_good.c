#include <stdio.h>

void func ( void );

void func ( void )
{
  long int long_var;
  short int short_a, short_b;
  short_a = short_b = 1;
  long_var = ( long )short_a + short_b; // explicitly converted to long
}