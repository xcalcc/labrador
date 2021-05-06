/*
 * GJB5111: 5.1.1.2
 * Define other something as keywords is forbidden
 */

#define JUDGE if  // incompliant

#define int64 long // compliant

int main(void) {
  int64 i = 0;
  JUDGE (0)
    return 0;
}
