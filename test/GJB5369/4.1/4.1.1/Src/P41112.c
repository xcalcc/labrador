/*
 * GJB5369: 4.1.1.2
 * identifier name reused as other purpose is forbidden
 */
void static_p(void)
{
  unsigned int value_x = 1u;
  unsigned int y = 0u;
value_x:  /* value_x reused as label is forbidden */
  y = 1u;
}

