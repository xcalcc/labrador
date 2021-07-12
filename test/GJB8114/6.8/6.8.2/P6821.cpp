/*
 * GJB8114: 6.8.2.1
 * Using default catch after other catches to avoid omitting
 */

class A {
};

int main(void) {
  try {
    A a;
    throw a;
  } catch (A &) {

  } catch (...) {

  }
}