/*
 * GJB8114: 6.1.1.4
 * Overloaded assigment operator in abstract classes should be private or protect.
 */

class B {
private:
  int kind;

public:
  B(void) : kind(0) {}

  virtual ~B(void) {}

  B &operator=(B const &rhs) {
    kind = rhs.kind;
    return (*this);
  }
};