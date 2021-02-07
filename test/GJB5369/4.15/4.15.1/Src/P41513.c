/*
 * GJB5369: 4.15.1.3
 * formal parameter's name should be different from global variable
 */

unsigned int global_int = 0;

void static_p(unsigned int *global_int){
    /* ... */
}