/*
 * GJB5369: 4.7.1.5
 * using extern in a file which extern is forbidded
 */

/* any extern to forbidded */
extern unsigned int undef_global;
void static_p(void) { undef_global = 1; }