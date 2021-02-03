/*
 * GJB5369: 4.1.1.16
 * redefing a name is foridden
 */

unsigned int static_p(void);

/* redefined the name "static_p" */
struct static_p{
    unsigned int static_p;  /* redefined the name "static_p" */
    unsigned int u_1;
};  

unsigned int static_p(void) {
    unsigned int var_1;

static_p:   /* redefined the name "static_p" is forbidden */
    
    var_1 = 1u;
    return (var_1); 
}