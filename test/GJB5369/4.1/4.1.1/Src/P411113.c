/*
 * GJB5369: 4.1.1.13
 * keywords in macro is forbidden
 */

/* Keywords in macro is forbidden */
#define static_p unsigned int

void test_p(void) {
    static_p x = 1u;
}