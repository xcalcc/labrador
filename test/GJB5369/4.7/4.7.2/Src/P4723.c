/*
 * GJB5369: 4.7.2.3
 * use abort/exit carefully
 */

void static_p(bool flag) {
  if (flag) {
    abort();
  }
  exit(0);
}