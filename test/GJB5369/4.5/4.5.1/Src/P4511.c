/*
 * GJB5369: 4.5.1.1
 * jump out from a function is forbidden
 */

#include <setjmp.h>

static void static_p(jmp_buf mark, unsigned int val){
    /* jump out from a function is forbidden */
    longjump(mark, val);
}