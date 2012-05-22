#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include "linalg.h"

template <typename TData>
TData pow2(const TData& a) {
  return a * a;
}

template <typename TData>
class Function {
public:
  TData operator()(const std::vector<TData> &data) const {
    return std::exp((data[0] * data[1] + data[1] * data[2] + data[2] * data[0]) / 2.0) *
        std::exp((pow2(data[0]) * data[1] + pow2(data[1]) * data[2] + pow2(data[2]) * data[0] + data[0] * data[1] * data[2]) / 2.0) *
        std::log(1.0 + data[0] + data[1] + data[2] + std::log(1.0 + pow2(data[0]) + pow2(data[1]) + pow2(data[2])));
  }
};

template <typename TData, typename TFunc>
class FuncMapper {
public:
  FuncMapper(const TFunc &func, const std::vector<TData> &data)
      : data_(data) // copying!!!
      , func_(func) {
    // insert last mutable cell
    data_.push_back(0.0);
  }

  TData operator() (const TData& t) const {
    data_.back() = t; // modify mutable cell
    return func_(data_);
  }

private:
  mutable std::vector<TData> data_;
  const TFunc &func_;
};

template <typename TData, typename TFunc>
TData base_sum(const TFunc &func, const TData& low, const TData& high, size_t count) {
  TData result = 0;

  result -= func(low) * 0.5;
  for (size_t i = 0; i < count; ++i) {
    result += func(low + ((high - low) * i ) / count);
  }
  result += func(high) * 0.5;

  result *= (high - low) / count;
  return result;
}

template <typename TData, typename TFunc>
TData romberg_sum(const TFunc &func, const TData& low, const TData& high, size_t count, TData *error_ptr = NULL) {
  size_t chunk_count = count;

  std::vector<TData> product, product_copy;
  std::vector< std::vector<TData> > matrix, matrix_copy;
  const size_t ITER_COUNT = 5;
  for (size_t i = 0; i < ITER_COUNT; ++i, chunk_count /= 2) {
    TData value = base_sum<TData, TFunc>(func, low, high, chunk_count);
    product.push_back(value);
    std::vector<TData> row;
    for (size_t j = 0; j < ITER_COUNT; ++j) {
      row.push_back(std::pow(count / chunk_count, 2.0 * j));
    }
    matrix.push_back(row);
  }

  std::vector<TData> coefs;
  matrix_copy = matrix;
  product_copy = product;
  // cut the most precise sum
  product_copy.erase(product_copy.begin());
  matrix_copy.erase(matrix_copy.begin());
  for (size_t i = 0; i < matrix_copy.size(); ++i) {
    matrix_copy[i].pop_back();
  }

  solve(matrix_copy, product_copy, coefs);
  TData sub_value = coefs[0];

  solve(matrix, product, coefs);

  if (error_ptr != NULL) {
    *error_ptr = sub_value - coefs[0];
  }

  return coefs[0];
}

template <typename TData, typename TFunc>
class Functor {
public:
  Functor(const TFunc& func, const TData &t_0, const TData &t_1, size_t count)
    : t_0_(t_0)
    , t_1_(t_1)
    , count_(count)
    , func_(func) {}
  
  TData operator () (const std::vector<TData> &data, TData *error_ptr = NULL) const {
    return romberg_sum(FuncMapper<TData, TFunc>(func_, data), t_0_, t_1_, count_, error_ptr);
  }
  TData operator () () const {
    std::vector<TData> data; 
    TData error;
    TData result = this->operator()(data, &error);
    std::cout << "error: " << error << std::endl;
    return result;
  }
private:
  TData t_0_;
  TData t_1_;
  size_t count_;
  const TFunc &func_;
};

template <typename TData, typename TFunc>
Functor<TData, TFunc> make_functor(const TFunc& func, const TData &t_0, const TData &t_1, size_t count) {
  return Functor<TData, TFunc>(func, t_0, t_1, count);
}

int main() {

  typedef double TData;
  TData z_0(0.0), z_1(1.0);
  TData y_0(0.0), y_1(1.0);
  TData x_0(0.0), x_1(1.0);
  size_t c = 128; // number of chunks

  // write integral expressions like humans do
  TData result = make_functor(make_functor(make_functor(Function<TData>(), z_0, z_1, c), y_0, y_1, c), x_0, x_1, c)();
  
  std::cout.precision(16);
  std::cout << "res: " << result << std::endl;

  std::ofstream os("outfile.txt");
  os.precision(16);
  os << result << std::endl;
  os.close();

  return 0;
}
