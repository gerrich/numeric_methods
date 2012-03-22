#include <iostream>
#include "precise.h"

int main() {
  precise<10> a(0.111111111111111111111111111111), b(-0.1111111111111111111111111111111111);
  std::cout << double(a) << std::endl;
  std::cout << double(b) << std::endl;
  a*=b;
  std::cout << double(a) << std::endl;
  return 0;
}
