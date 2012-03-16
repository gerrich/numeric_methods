#pragma once

typedef unsigned long ulong;

const ulong FACTOR_B = 1664525;
const ulong FACTOR_A = 1013904223;
const ulong INITIAL_FACTOR = 111;
const size_t STEP_SIZE = 100000;


static void init_data(std::vector<ulong>& data, size_t size, ulong alpha) {
  data.resize(size);
  
  data[0] = INITIAL_FACTOR * alpha;
  for (size_t i = 1; i < size; ++i) {
    data[i] = FACTOR_B * data[i-1] + FACTOR_A;
  }
}

static void print_result(std::ostream& os, std::vector<ulong>& data) {
  for (size_t i = 0; i < data.size(); i += STEP_SIZE) {
    os << data[i] << std::endl;
  }
}


