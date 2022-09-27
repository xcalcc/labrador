/*
 * GJB5369: 4.11.1.2
 * loop value should be local value
 */

/* loop value should be local value */
unsigned int global_f = 0u;

int loop_standards(int p_1) {
  int j = 10;
  /* ... */
  for (global_f = 0; global_f < 10; global_f = global_f + 1) {
    j--;
  }
  return j;
}