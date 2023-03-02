#include <stdio.h>

extern inline int func( void ); // inline function declared with external linkage
int func2( void );

int func2( void )
{
  func();
}