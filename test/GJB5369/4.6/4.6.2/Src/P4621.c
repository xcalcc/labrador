/*
 * GJB5369: 4.6.2.1
 * avoid using ',' operator
 */

void static_p(void) {
    unsigned int x = 1u;
    x++, x+=1; /* avoid using ',' operator */
}