// $Id: A13-3-1.cpp 309903 2018-03-02 12:54:18Z christof.meerwald $

#include <cstdint>

template<typename T>
void F1(T &&t) noexcept(false) {}

// Non-compliant - overloading a function with forwarding reference
void F1(int &&t) noexcept {}

template<typename T>
void F2(T &&t) noexcept(false) {
}

void F2(int &) = delete; // Compliant by exception


int main(int, char **) {
  std::int32_t x = 0;
  F1(x); // Calls f1(T&&) with T = int& F1(+x); // Calls f1(std::int32_t&&)
  F1(0); // Calls f1(std::int32_t&&)
  F1(0U); // Calls f1(T&&) with T = unsigned int F2(0); // Calls f2(T&&) with T = int
// f2(x);  // Compilation error, the overloading function is deleted
}