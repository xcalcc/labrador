/*
 * GJB5369: 4.1.2.1
 * Use typedef redefine the basic type
 */

typedef unsigned int    UINT_32;
typedef int             SINT_32;
typedef unsigned short  UINT_16;
typedef unsigned char   UCHAR;
typedef float           FLOAT_32;
typedef double          FLOAT_64;

void p(void) {
  UINT_32 i = 0u;
  FLOAT_32  x = 0x0f;
}