#include <vector>

#include <iostream>
#include <fstream>

#include "insertions_sort.h"
#include "common.h"


const size_t DATA_SIZE = 1e7;
const ulong ALPHA = 42;

int main() {
  std::vector<ulong> data;

  init_data(data, DATA_SIZE, ALPHA);

  insertion_sort(data.begin(), data.end());  // To long to wait for. O(n^2) complexity

  std::ofstream os("outfile.txt");
  print_result(os, data);

  return 0;
}
