/*
 * GJB8114: 5.1.1.8
 * identifier is a must in declaration of struct, enum and union
 */

struct {
  int data1;
} sData;

union {
  int a;
} uData;

enum {
  A = 0,
  B, C, D
} eLevel;

int main(void) {
  eLevel = B;
  sData.data1 = 2000;
  uData.a = 2;
  return 0;
}