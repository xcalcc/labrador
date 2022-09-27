/*
 * GJB8114: 5.1.1.9
 * Bit-fields signed number should longer than 1 bit
 */

typedef struct {
  signed int s1: 1;  // incompliance
  signed int s2: 2;
} sData;

int main(void) {
  sData myData;
  myData.s1 = 2;
  myData.s2 = 3;
}