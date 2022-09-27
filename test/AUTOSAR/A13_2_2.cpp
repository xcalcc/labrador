// $Id: A13-2-2.cpp 271687 2017-03-23 08:57:35Z piotr.tanski $
class A {
};

// Compliant
A operator+(A const &, A const &) noexcept {
  return A{};
}

// Compliant
int operator/(A const &, A const &) noexcept {
  return 0;
}

// Compliant
A operator&(A const &, A const &) noexcept {
  return A{};
}

// Non-compliant
const A operator-(A const &, int) noexcept { return A{}; }

// Non-compliant
A *operator|(A const &, A const &) noexcept {
  return new A{};
}