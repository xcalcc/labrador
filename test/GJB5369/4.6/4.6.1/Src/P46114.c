/*
 * GJB5369: 4.6.1.14
 * overflow should be avoided
 */

int static_p(void) {
    int a = 2147483646;
    int b= 2147483647;
    int c = a + b;  /* a + b overflowed */
    return c;
}