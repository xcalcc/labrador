/*
 * GJB8114: 5.8.1.2
 * Useless statement is forbidden
 */

int main(void) {
  unsigned int local = 0;
  unsigned int para = 0;
  local;
  para - 0;
  local == para;
  local > para;
  return 0;
}