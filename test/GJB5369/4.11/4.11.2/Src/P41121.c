/*
 * GJB5369: 4.11.2.1
 * avoid using infinite loop
 */

void static_p(void) {
  for (;;)
    ;

  while (1) {
  }
  do {
  } while (1);
}