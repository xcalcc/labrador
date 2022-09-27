#include <stdio.h>
#include <malloc.h>

void func(void);

void func(void) {
  // const pointer declared
  int *const ptr_const = (int *) malloc(sizeof(int));
  int *ptr_int;
  // ptr_int point to the address of ptr_const
  ptr_int = ptr_const;
  // more code that may change ptr_const
  // ...

}