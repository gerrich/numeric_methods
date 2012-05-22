#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include "linalg.h"
#include "utils.h"

//u(x) - Int(a, b, K(x,t) * u(t) * dt) = f(x)

template <typename TData>
class Function {
public:
  TData free_part(const TData &x) const {
    // cheat to avoid null devision
    if (abs(x) < 1e-5) {
      return 1.0;
    } 
    return 1.0 + 1/x * (cos(0.5 * x) - 1);
  }

  TData kernel(const TData &x, const TData &t) const {
    if (t > 0.5 + 1e-10) { // cheat: integrate on [0.0 .. 0.5]
      return 0.0;
    } 
    return sin(x * t);
  }
};


template <typename TData, template <class> class TFunc>
void make_matrices(
    const TFunc<TData> &func,
    const std::vector<TData> &x_data,
    std::vector< std::vector<TData> > &matrix, 
    std::vector<TData> &result) {

  result.resize(x_data.size());
  for (size_t i = 0; i < x_data.size(); ++i) {
    result[i] = func.free_part(x_data[i]);
  }

  matrix.resize(x_data.size(), std::vector<TData>(x_data.size(), 0.0));
  for (size_t out_index = 0; out_index < x_data.size(); ++out_index) {
    matrix[out_index][out_index] = 1.0;
    for (size_t in_index = 0; in_index < x_data.size(); ++in_index) {
      TData dt = 0.0;
      if (in_index > 0) {
        dt += 0.5 * (x_data[in_index] - x_data[in_index - 1]);
      }
      if (in_index + 1 < x_data.size()) {
        dt += 0.5 * (x_data[in_index + 1] - x_data[in_index]);
      }
      matrix[out_index][in_index] -= func.kernel(x_data[out_index], x_data[in_index]) * dt;
    }
  }
}

int main() {
  typedef double TData;
  
  Function<TData> func;
  std::vector<TData> x_data, u_data, free_part;
  std::vector< std::vector<TData> > matrix;
  TData step = 0.01;

  make_x_data(0.0, 1.0, step, x_data);
  size_t cheat_index = 0.5 / step;
  x_data.insert(x_data.begin() + cheat_index, 0.5 + 1e-9); //cheat to minimize error on incontineous kernel

  make_matrices(func, x_data, matrix, free_part);
  solve(matrix, free_part, u_data);

  // remove extra data point
  x_data.erase(x_data.begin() + cheat_index); 
  u_data.erase(u_data.begin() + cheat_index);

  // analitic solution u(x) == 1.0;
  std::vector<TData> canonical_data(u_data.size(), 1.0);

  std::cout << "max deviation: " << major_diff(u_data, canonical_data) << std::endl; 

  std::ofstream os("outfile.txt");
  os.precision(10);
  print_data(os, x_data, u_data);
  os.close();

  return 0;
}
