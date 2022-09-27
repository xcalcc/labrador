/*
 * GJB8114: 5.1.2.3
 * Using ## and # carefully in macro
 */

#define STRING(a) #a
#define CONCAT(a, b) a##b

int main() {

}