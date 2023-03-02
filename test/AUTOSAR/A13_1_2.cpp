// $Id: A13-1-2.cpp 289436 2017-10-04 10:45:23Z michal.szczepankiewicz $
// Compliant
constexpr long double operator "" _m(long double meters) {
  return meters;
}

// Compliant
constexpr long double operator "" _kg(long double kilograms) {
  return
      kilograms;
}

// Implementation
constexpr long double operator "" m(long double meters) // Non-compliant
{
  return meters;
}

constexpr long double operator "" kilograms(
    long double kilograms) // Non-compliant
{
// Implementation
  return kilograms;
}

void Fn() {
  long double weight = 20.0_kg;
  long double distance = 204.8_m;
}