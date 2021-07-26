#include <stdio.h>

static inline int func( void ); // inline function declared with internal linkage
int func2( void );

int func2( void )
{
  func();
}