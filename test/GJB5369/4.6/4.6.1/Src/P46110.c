/*
 * GJB5369: 4.6.1.10
 * array index must be int
 */

#define ArraySize 3.0f

void static_p(void) {
    /* array index must be int */
    unsigned int f1_arr[ArraySize] = {0, 1, 2};
}