/* MISRA
 * Rule: 5.6
 * A typedef name shall be a unique identifier
 */


#include "5_6.h"

static void func (void)
{
  {
    typedef unsigned char u8_t;

    u8_t c1 = get_uint8 ( );
    use_uint8 ( c1 );
  }
  {
    typedef unsigned char u8_t; /* Non-compliant */

    u8_t c2 = get_uint8 ( );
    use_uint8 ( c2 );
  }
}


typedef float mass32;

static void func1 (void)
{
  float32_t mass32 = 0.0f;       /* Non-compliant, also breaks R.5.3 */

  use_float32 ( mass32 );
}


typedef struct list
{
  struct list *next;
  int     element;
} list;                           /* Compliant - exception */

typedef struct
{
  struct chain
  {
    struct chain *chlist;
    int      element;
  } s1;
  int length;
} chain;                          /* Non-compliant - tag "chain" not associated with typedef */


void R_5_6 ( void )
{
  const mass32 zero_mass = 0.0F;
  chain cable = { 0 };
  list list1 = { 0, 1U };

  func ( );
  func1 ( );
  use_float32 ( zero_mass );
  use_uint16 ( cable.s1.element + cable.length );
  use_uint16 ( list1.element );
}

/* end of R_05_06.c */