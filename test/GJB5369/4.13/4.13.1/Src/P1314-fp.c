unsigned int bad(unsigned int b, unsigned int c) {
  unsigned int a;
  a = b & 0xFF - c;
  return a;
}

unsigned int good(unsigned int b, unsigned int c) {
  unsigned int a;
  a = (b & 0xFF) - c;
  return a;
}