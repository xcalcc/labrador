/*
 * GJB5369: 4.1.1.17
 * self-defined types(typedef) redefined as other types is forbidden 
 */

typedef int mytype;

void static_p(void) {
    typedef float mytype;   /* Redefining the "mytype" is forbidden. */
}