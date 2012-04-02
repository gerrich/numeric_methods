#include <iostream>
#include "complex.h"


int main() {

  Complex c1(10, 10);
  Complex c2(2, 7);

  std::cout << c1 * c2 << std::endl;
  std::cout << c1 / c2 << std::endl;

  return 0;
}
