/*
 * GJB5369: 4.6.1.18
 * bit-wise operation is forbidden in the logical expression
 */

void static_p(void) {
    unsigned int y = 2u;
    bool flag = false;

    /* bit-wise operation is forbidden in the logical expression */
    flag = flag & (y == 2u);
}