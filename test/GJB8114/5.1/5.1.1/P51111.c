/*
 * GJB8114: 5.1.1.11
 * Signed type should be explicitly declared with bit-fields
 */

typedef struct {
  short d1: 2;    // incompliance
  short d2: 2;    // incompliance
  short res: 12;  // incompliance
} sData;

int main(void) {

}