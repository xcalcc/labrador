#include <stdio.h>

// restrict-qualified parameters declared
extern void func (int * restrict ptr1, int * restrict ptr2);
void func2 (void);

void func2(void)
{
  int i;
  int *a,*b;
  a = b = &i;
  // overlap memory areas
  func (a, b);
}