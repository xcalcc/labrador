/*
 * GJB5369: 4.1.1.1
 * procedure name reused as other purpose is forbidden
 */
void foo(unsigned int p_1)
{
  unsigned int x = p_1;
}

void static_p(void)
{
  unsigned int foo = 1u;  /* foo reused as variable is forbidden */
}

