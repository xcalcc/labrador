// $Id: A11-0-2.cpp 289436 2017-10-04 10:45:23Z michal.szczepankiewicz $
#include <cstdint>

// Compliant
struct A {
  std::int32_t x;
  double y;
};

// Compliant
struct B {
  std::uint8_t x;
  A a;
};

// Compliant
struct C {
  float x = 0.0f;
  std::int32_t y = 0;
  std::uint8_t z = 0U;
};

// Non-compliant
struct D {
public:
  std::int32_t x;
protected:
  std::int32_t y;
private:
  std::int32_t z;
};

// Non-compliant
struct E {
public:
  std::int32_t x;

  void Fn() noexcept {}

private:

  void F1() noexcept(false) {}

};

// Non-compliant
struct F : public D {
};