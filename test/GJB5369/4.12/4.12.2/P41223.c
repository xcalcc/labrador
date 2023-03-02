/*
 * GJB5369: 4.12.2.3
 * pay attention to the type of operator of binocular operation
 */

void static_p(int i) {
  int x = 1;
  float y = 2.0f;
  unsigned int z = 3u;

  /* The type of y and z is different from x. */
  x = (i == 0) ? y : z;
}