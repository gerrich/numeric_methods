#include <vector>

#include <iostream>
#include <fstream>

#include "qsort.h"
#include "common.h"

const size_t DATA_SIZE = 1e7;
const ulong ALPHA = 42;

int main() {
  std::vector<ulong> data;

  init_data(data, DATA_SIZE, ALPHA);

  qsort(data.begin(), data.end());

  std::ofstream os("outfile.txt");
  print_result(os, data);

  return 0;
}
