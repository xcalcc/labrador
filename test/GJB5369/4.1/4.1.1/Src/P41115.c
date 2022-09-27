/*
 * GJB5369: 4.1.1.5
 * declaring the type of parameters is a must
 */

unsigned int static_p(p_1) { /* declaring the type of p_1 is a must */
    unsigned int result;
    result = p_1 * 2;
    return result;
}