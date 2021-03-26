/*
 * GJB5369: 4.7.1.10
 * formal and real parameters' type should be the same
 */

bool static_p(float up_1);

bool static_p(int p_1) {
  return false;
}

int main() {
  return 0;
}