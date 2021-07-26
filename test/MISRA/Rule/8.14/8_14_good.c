#include <stdio.h>

// restrict type qualifier removed
extern void func (int * ptr1, int * ptr2);
void func2 (void);

void func2(void)
{
  int i;
  int *a,*b;
  a = b = &i;
  func (a, b);
}