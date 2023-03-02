const char *p;

char *init_array(void) {
  char array[10];
  /* Initialize array */
  return array;
}

void dont_do_this(void) {
  const char c_str[] = "This will change";
  p = c_str; /* Dangerous */
}