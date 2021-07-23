#include <stdio.h>

struct str
{
  unsigned int a:1;   // single-bit bit field declared unsigned
};

int main()
{
  printf ("finished") ;
  return 0;
}