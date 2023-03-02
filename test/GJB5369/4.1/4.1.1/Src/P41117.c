/*
 * GJB5369: 4.1.1.7
 * only type but no identifiers in function prototype is forbidden
 */

struct s_type_b {
  unsigned int xs;
};

/* without parameter identifier */
void static_p_a(unsigned int p_1, struct s_type_b *);
void static_p_a(unsigned int p_1, struct s_type_b *) {

}