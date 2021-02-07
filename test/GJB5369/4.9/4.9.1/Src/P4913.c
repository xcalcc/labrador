/*
 * GJB5369: 4.9.1.3
 * the return statement must have return value
 */

unsigned int static_p(unsigned int p_1, unsigned short p_2) {
    return; /* return statement without return value is forbidden */
}