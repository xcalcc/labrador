#include <stdint.h>

void func(void);

void func(void) {
  uint32_t uint32_var = 2;
  // 17 is in the range 0 to the size of uint32_var
  uint32_var = uint32_var << 17;
}