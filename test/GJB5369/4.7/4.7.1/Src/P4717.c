/*
 * GJB5369: 4.7.1.7
 * function return void used in statement is forbidden
 */

void foo(void) { /* ... */
}

void static_p(void) {
  char x;
  /* function return void used in statement is forbidden */
  x = (char)foo();
}