/*
 * GJB8114: 6.1.1.2
 * Virtual base class converting to derived class should use dynamic_cast
 */

class B {
  int a;
public:
  virtual ~B() {}
};

class D : public virtual B {
  int c;
public:
  virtual ~D() {}
};

int main(void) {
  D d;
  B b;
  B *pb = &b;

  D *pd1 = dynamic_cast<D*>(pb);      // compliance
  D &pd2 = dynamic_cast<D&>(*pb);     // compliance
  D *pd3 = reinterpret_cast<D*>(pb);  // incompliance
  D &pd4 = reinterpret_cast<D&>(*pb); // incompliance
}