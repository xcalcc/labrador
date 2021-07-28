#include <stdio.h>

void func ( void );

void func ( void )
{
  long int long_var;
  short int short_a, short_b;
  short_a = short_b = 1;
  long_var = short_a + short_b; // implicitly converted to long
}