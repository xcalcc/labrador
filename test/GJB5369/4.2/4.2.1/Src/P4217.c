/*
 * GJB5369: 4.2.1.7
 * the assemble procedure should be pure assemble
 */

void static_p(void) {
  unsigned int x;
  x = 0u;

  /* the assemble procedure should be pure assemble */
  __asm__ (
  "mov eax, x;"
  );
}