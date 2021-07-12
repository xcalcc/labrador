/*
 * GJB8114: 6.8.1.2
 * Each specified throw must have a matching catch
 */

class A {

};

class B {
};

int main(void) {
  int i = 2;
  try {
    if (i > 3) {
      throw A();
    } else {
      throw B();
    }
  } catch (A &) {

  } // imcompliance: missing catch "throw B();"
}