/*
 * GJB5369: 4.4.1.3
 * function pointer is forbidden
 */

void foo(unsigned int p_1, unsigned short p_2) {}

void static_p(void){
    /* Function pointer is forbidden. */
    void (*proc_pointer)(unsigned int, unsigned short) = foo;
    proc_pointer(1u, 1);
}