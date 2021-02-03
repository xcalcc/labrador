/*
 * GJB5369: 4.1.1.15
 * the sign of the char type should be explicit
 */

void static_p(void) {
    char c='c'; /* The sign of the char is not explicit. */
    /* ... */
}