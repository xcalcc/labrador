#include <stdio.h>
#include <malloc.h>

void func(void);

void func(void) {
  // pointer to const declared
  const int *ptr_const = (int *) malloc(sizeof(int));
  int *ptr_int;
  // const qualifier removed in a cast
  ptr_int = (int *) ptr_const;
  // more code that may change ptr_const
  // ...

}