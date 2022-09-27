// $Id: A10-3-5.cpp 289436 2017-10-04 10:45:23Z michal.szczepankiewicz $
class A {
public:
  virtual A &operator=(A const &oth) = 0; // Non-compliant
  virtual A &operator+=(A const &rhs) = 0; // Non-compliant
};

class B : public A {
public:
  // It needs to take an argument of type
  // A& in order to override
  B &operator=(A const &oth) override {
    return *this;
  }

  // It needs to take an argument of type A& in order to override
  B &operator+=(A const &oth) override {
    return *this;
  }

  B &operator-=(B const &oth) // Compliant
  {
    return *this;
  }
};

class C : public A {
public:
  // It needs to take an argument of type
// A& in order to override
  C &operator=(A const &oth) override {
    return *this;
  }

  // It needs to take an argument of
  // type A& in order to override
  C &operator+=(A const &oth) override {
    return *this;
  }

  C &operator-=(C const &oth) // Compliant
  {
    return *this;
  }
};