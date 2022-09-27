/*
 * GJB5369: 4.13.1.3
 * nested structure should stay the same with the struct
 */

struct pixel {
  unsigned int colour;
  struct {
    unsigned int x, y;
  } coords;
};

void static_p(void) {
  unsigned int xs = 0u;
  unsigned int ys = 0u;
  
  struct pixel spot = {1u, xs, ys};
}