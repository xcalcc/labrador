/*
 * GJB5369: 4.6.1.8
 * The value assigned to a variable should be the same type
 * as the variable
 */

void static_p(void) {
    unsigned int d;
    d = 2.0;    /* Requires explicit assignment of 2u */
}