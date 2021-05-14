/*
 * GJB8114: 5.1.1.9
 * Anonymous struct in struct is forbidden
 */

struct SData {
  int id;
  struct Scoor {
    int xs;
    int ys;
    int zs;
  };
};

int main(void) {
  struct SData data;
  data.id = 9;
  return 0;
}