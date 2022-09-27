/*
 * GJB5369: 4.15.1.4
 * formal parameter's name should be different with typename or identifier
 */

typedef unsigned int DUPLICATE;

void static_p(unsigned int DUPLICATE){
    /* ... */
}