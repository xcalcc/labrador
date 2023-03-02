/*
 * GJB5369: 4.7.1.3
 * static type function should be called within the file
 */


/* This static function will never been called should be deleted. */
static bool static_p(unsigned int p_1) {
  bool ret = false;
  unsigned int i = p_1 + 1u;
  if (i == 0) {
    ret = true;
  }
  return ret;
}

void main(void) {}