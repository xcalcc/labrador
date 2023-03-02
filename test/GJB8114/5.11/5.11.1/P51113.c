/*
 * GJB8114: 5.11.1.3
 * Initial value of struct variable should keep the nested structure up with the declaration
 */

struct Spixel {
  unsigned int color;
  struct coord {
    unsigned int x;
    unsigned int y;
  } coords;
};

int main(void) {
  struct Spixel pixel = {1, 2, 3};
}