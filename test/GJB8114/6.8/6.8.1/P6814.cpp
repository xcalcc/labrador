/*
 * GJB8114: 6.8.1.4
 * Default catch should stays behind other all catches
 */

class A {
};

int main(void) {
  try {
    A a;
    throw a;
  } catch (...) {

  } catch (A &) {

  }
}