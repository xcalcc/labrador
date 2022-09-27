/*
 * GJB5369: 4.1.1.11
 * Using '#' and '##' in the same macro is forbidden
 */

#define TEST(a, b) #a##b /* using # and ## in one macro is forbidden */

#define TEST1(a, b) a##b##b /* using ## more than one time in one macro */

#define TEST2(a, b) #a#a /*using # more than one time in one macro */