/*
 * GJB8114: 5.5.1.2
 * Using setjmp/longjmp is forbidden
 */

#include <setjmp.h>

static void static_p(jmp_buf mark, unsigned int val){
  longjmp(mark, val);
  setjmp(mark);
}