/*
 * GJB8114: 6.8.1.3
 * Exception objects should be catched as reference
 */

class A {
};

int main(void) {
  try {
    A a;
    throw &a;
    throw a;
  } catch (A &) {

  } catch (A *) { // imcompliance

  }
}