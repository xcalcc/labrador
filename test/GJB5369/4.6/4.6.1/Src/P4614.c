/*
 * GJB5369: 4.6.1.4
 * shifting variable should not overflow
 */

void static_p(void) {
    unsigned int x = 0u;
    unsigned int y = 2u;
    
    /* shifting variable should not overflow */
    x = y << 34;
}