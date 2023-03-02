#include <stdio.h>

void func ( void );

void func ( void )
{
  char char_var;
  bool bool_var;
  // int can be cast to char
  char_var = ( char ) 1;
  // int 1 can be cast to bool by exception
  bool_var = ( bool ) 1;
}