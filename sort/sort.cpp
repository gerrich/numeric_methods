#include <vector>

#include <iostream>
#include <fstream>


typedef unsigned long ulong;

void init_data(std::vector<ulong>& data, size_t size, ulong alpha) {
  data.resize(size);
  
  data[0] = 111 * alpha;
  for (size_t i = 1; i < size; ++i) {
    data[i] = 1664525 * data[i-1] + 1013904223;
  }
}

void print_result(std::ostream& os, std::vector<ulong>& data) {
  const size_t STEP_SIZE = 100000;
  for (size_t i = STEP_SIZE; i < data.size(); i += STEP_SIZE) {
    os << data[i] << std::endl;
  }
}

template <typename TIter>
void qsort(TIter begin, TIter end) {
   
}

int main() {

  std::vector<ulong> data;

  init_data(data, 1e7, 42);

  qsort(data.begin(), data.end());

  print_result(std::cout, data);

  return 0;
}
