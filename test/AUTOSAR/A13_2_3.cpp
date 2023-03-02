class A {
};

// Compliant
bool operator==(A const &, A const &) noexcept {
  return true;
}

// Compliant
bool operator<(A const &, A const &) noexcept {
  return false;
}

// Compliant
bool operator!=(A const &lhs, A const &rhs) noexcept {
  return !(operator==(lhs, rhs));
}

// Non-compliant
int operator>(A const &, A const &) noexcept {
  return -1;
}

// Non-compliant
A operator>=(A const &, A const &) noexcept {
  return A{};
}

// Non-compliant
const A &operator<=(A const &lhs, A const &rhs) noexcept {
  return lhs;
}