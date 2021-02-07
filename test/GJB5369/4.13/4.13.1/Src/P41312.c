/*
 * GJB5369: 4.13.1.2
 * inital value type of struct should stay the same with struct
 */

struct s_type_a {
  int xs;
  float fs;
};

void static_p(void) {
    /* The type of xs is float. */
  struct s_type_a_sta {
    3.14f, 0.0f
  };
}