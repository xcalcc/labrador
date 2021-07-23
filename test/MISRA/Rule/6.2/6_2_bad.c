#include <stdio.h>

struct str
{
  signed int a:1;   // single-bit bit field declared signed
};

int main()
{
  printf("finished");
  return 0;
}