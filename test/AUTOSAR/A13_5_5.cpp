// $Id: A13-5-5.cpp 325916 2018-07-13 12:26:22Z christof.meerwald $
#include <cstdint>

class A {
public:
  explicit A(std::uint32_t d) : m_d(d) {}

  // Non-compliant: member, not noexcept
  bool operator==(A const &rhs) const {
    return m_d == rhs.
        m_d;
  }

  std::uint32_t m_a;

private:
  std::uint32_t m_d;
};

class C {
public:
  operator A() const;
};

void Foo(A const &a, C const &c) {
  a == c; // asymmetric as "a ==c" compiles, but "c == a" doesn’t compile
}

class B {
public:
  explicit B(std::uint32_t d) : m_d(d) {}

  // Compliant: non-member, identical parameter types, noexcept
  friend bool operator==(B const &lhs, B const &rhs) noexcept {
    return lhs.m_d == rhs.m_d;
  }

  // Non-compliant: non-member, identical parameter types
  friend bool operator!=(B const &lhs, B const &rhs) {
    return lhs.m_d != rhs.m_d;
  }

  // Non-compliant: non-member, noexcept
  friend bool operator==(B const &lhs, A const &rhs) noexcept {
    return lhs.m_d == rhs.m_a;
  }

private:
  std::uint32_t m_d;
};

class D {
public:
  operator B() const;
};

void Bar(B const &b, D const &d) {
  b == d;
  d == b;
}