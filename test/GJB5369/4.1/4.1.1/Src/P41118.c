/*
 * GJB5369: 4.1.1.8
 * ellipsis in the function's parameter list is forbidden
 */

/* ellipsis in the parameter list is forbidden */
unsigned int static_p(unsigned char *p_1, ...) {
  /* ... */;
  return 1u;
}