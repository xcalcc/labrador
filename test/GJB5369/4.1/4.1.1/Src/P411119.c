/*
 * GJB5369: 4.1.1.19
 * arrays without boundry limitation is forbidden
 */

void static_p(void) {
    unsigned int u_array[] = {0 ,1 ,2}; /* Array without size is forbidden. */
}