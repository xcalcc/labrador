/*
 * GJB8114: 6.8.1.5
 * Throwing NULL is forbidden
 */

#include <iostream>

int main(void) {
  try {
    throw NULL;
  } catch (int) {

  }
}