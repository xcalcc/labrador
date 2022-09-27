/*
 * GJB5369: 4.3.1.5
 * "switch" without statement is forbidden
 */

void static_p(int p_1) {
  int i = p_1;

  /* "switch" without statement is forbidden */
  switch (i) {}
}