/*
 * Release: 2016-11-01
 *
 * Example from MISRA C:2012 ( THIS IS NOT A TEST SUITE )
 *
 * Copyright HORIBA MIRA Limited.
 *
 * See file READ_ME.txt for full copyright, license and release instructions.
 */

/*
 * R.5.7
 *
 * A tag name shall be a unique identifier
 */


#include "5_7.h"

struct stag
{
  unsigned int a;
  unsigned int b;
};


struct deer
{
  unsigned int a;
  unsigned int b;
};

static void herd ( void )
{
  struct deer
  {
    unsigned int a;
  };           /* Non-compliant, also breaks R.5.3  */

  struct deer bambi = { 1U };
  use_uint16 ( bambi.a );
}


typedef struct coord
{
  int x;
  int y;
} coord;        /* Compliant by Exception */


struct elk
{
  int x;
};

struct elk      /* Non-compliant - Constraint violation in C99 */
{
  unsigned int x;
};


void R_5_7 ( void )
{
  struct stag a1 = { 0, 0 }; /* Compliant     */
  union  stag a2 = { 0, 0 }; /* Non-compliant - Constraint Violation in C99  */

  use_int16 ( a1.a + a1.b );
  use_int16 ( a2.a + a2.b );
  coord c1 = { 1U, 2U };
  struct coord c2 = { 3U, 4U };
  use_uint16 ( c1.x + c1.y + c2.x + c2.y );
  struct deer rudolph = { 1U, 20U };
  use_uint16 ( rudolph.a + rudolph.b );
  struct elk ellie = { 0U };
  use_uint32 ( ellie.x );

  herd ( );
}

/* end of R_05_07.c */