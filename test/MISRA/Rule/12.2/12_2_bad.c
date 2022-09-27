#include <stdint.h>

void func(void);

void func(void) {
  uint16_t uint16_var = 2;
  // 17 is bigger than the size of uint16_var
  uint16_var = uint16_var << 17;
}