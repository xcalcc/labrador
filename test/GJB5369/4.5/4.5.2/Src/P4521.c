/*
 * GJB5369: 4.5.2.1
 * setjmp/longjmp is forbidden
 */

#include <setjmp.h>

static void static_p(jmp_buf mark, unsigned int val) {
  /* setjmp/longjmp is forbidden */
  longjmp(mark, val);
}