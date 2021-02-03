/*
 * GJB5369: 4.1.1.3
 * struct with empty field is forbidden
 */

struct s_p {
  unsigned int xs;
  struct {
    unsigned char ac, ab;
  };
};

void static_p(void) {
    struct s_p sb;
    sb.xs = 1;
}