/*
 * GJB5369: 4.9.1.4
 * type of return value should stay the same
 */

unsigned int static_p(unsigned int par_1) {
  switch (par_1) {
  case 0:
    return (-1);
    break;
  case 1:
    return (1u);
    break;
  case 2:
    return (1L);
    break;
  case 3:
    return (1.0f);
    break;
  default:
    break;
  }
}