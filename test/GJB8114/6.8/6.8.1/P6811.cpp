/*
 * GJB8114: 6.8.1.1
 * The order of catch must be sorted from derived class to base class
 */

class A {

};

class B : public A {
};

class C : public A {
};

int main(void) {
  B b;
  C c;

  try {
    throw b;
  } catch (A &a) {

  } catch (B &b1) {

  }
}