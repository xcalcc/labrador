/*
 * GJB5369: 4.1.2.8
 * using "union" carefully
 */

/* union is dangerous as the coverage is uncertain */
union static_p {
  float fu;
  unsigned int xu;
};

void dummy(void) {}