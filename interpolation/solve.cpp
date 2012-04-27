#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "linalg.h"
#include "complex.h"

template <typename data_t>
void load_data(const char *fname, std::vector<data_t> &data_x, std::vector<data_t> &data_y) {
  std::ifstream f(fname);
  
  data_x.clear();
  data_y.clear();
  while(!f.eof()) {
    std::string line;
    if (!std::getline(f, line) or line.empty()) {
      continue;
    }
    std::stringstream ss(line);
    double x;
    double y;
    ss >> x >> y;
    data_x.push_back(data_t(x));
    data_y.push_back(data_t(y));
  }

  f.close();
}

template <typename data_t>
void train(const std::vector<data_t> &data_x, const std::vector<data_t> &data_y, std::vector<data_t> &coefs, size_t bottom_size) {
  std::vector< std::vector<data_t> > matrix;
  size_t size = data_x.size();
  for (size_t index = 0; index < size; ++index) {
    matrix.push_back(std::vector<data_t>());
    
    data_t value(1.0);
    for (size_t power = 0; power < size - bottom_size; ++power, value *= data_x[index]) {
      matrix.back().push_back(value);
    }
    value = -data_y[index] * data_x[index];
    for (size_t power = 1; power <= bottom_size; ++power, value *= data_x[index]) {
      matrix.back().push_back(value);
    }
  }
  std::vector<data_t> copy_y = data_y;
  solve(matrix, copy_y, coefs);
}

template <typename data_t>
data_t calc(const std::vector<data_t> &coefs, size_t bottom_size, const data_t &x) {
  size_t size = coefs.size();
  data_t top(0.0);
  data_t value(1.0);
  for (size_t power = 0; power < size - bottom_size; ++power, value *= x) {
    top += value * coefs[power];
  }
  data_t bottom(1.0);
  value = x;
  for (size_t power = 1; power <= bottom_size; ++power, value *= x) {
    bottom += value * coefs[size - bottom_size - 1 + power];
  }
  return top / bottom;
}

double real(double x) {
  return x;
}

double sqr(double a) {
  return a*a;
}

int main (int argc, char** argv) {
  typedef Complex data_t;
  std::vector<data_t> data_x;
  std::vector<data_t> data_y;
  char fname[] = "infile.txt";
  load_data(argc > 1 ? argv[1] : fname, data_x, data_y);

  // choose the devider polinom power here
  size_t bottom_size = data_x.size() / 2;

  std::vector<data_t> coefs;
  train(data_x, data_y, coefs, bottom_size);
 
  std::ofstream os("outfile.txt");
  for (size_t index = 0; index < data_x.size(); ++index) {
    Complex delta(0.0, 0.1);
    data_t new_y = calc(coefs, bottom_size, data_x[index] + delta);
    os << real(new_y) << std::endl;
  }
  os.close();

  return 0;
}
