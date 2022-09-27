/*
 * GJB8114: 5.1.1.12
 * Bit-fields should be the same length and within the length of its origin type
 */

typedef struct {
  unsigned char d1: 2;
  unsigned char d2: 2;
  unsigned char d3: 2;
  unsigned short d4: 4; // incompliance
} sData1;

typedef struct {
  unsigned short d1: 2;
  unsigned short d2: 2;
  unsigned short d3: 12; // incompliance
  unsigned short d4: 2;
} sData2;

int main(void) {
  sData1 d1;
  sData2 d2;
  return 0;
}