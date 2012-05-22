#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include "utils.h"

template <typename TData>
TData pow_2(const TData &x) {
  return x * x; 
}

//u(x) - Int(a, b, K(x,t) * u(t) * dt) = f(x)

template <typename TData>
class Function {
public:
  Function(const TData& lambda = 0.5) 
    : lambda_(lambda) {
  }

  TData free_part(const TData &x) const {
    return 1.0;
  }

  TData kernel(const TData &x, const TData &t) const {
    return lambda_ * pow_2(cos(x));
  }
private:
  const TData &lambda_;
};

template <typename TData, template <class> class TFunc>
class Integrable {
public:
  Integrable(
      const std::vector<TData> &x_data,
      const std::vector<TData> &data,
      const Function<TData> &func,
      const TData &arg)
    : x_data_(x_data)
    , data_(data)
    , func_(func)
    , arg_(arg) {
  }

  TData operator()(size_t index) const {
    return func_.kernel(arg_, x_data_[index]) * data_[index];
  }

private:
  const std::vector<TData> &x_data_;
  const std::vector<TData> &data_;
  const TFunc<TData> &func_;
  const TData &arg_;
};

template <typename TData, template <class> class TFunc>
TData calc_sum(const Integrable<TData, TFunc> &func, const std::vector<TData> &x_data) {
  TData result = 0.0;
  for (size_t i = 1; i < x_data.size(); ++i) {
    result += (x_data[i] - x_data[i - 1]) * 0.5 * (func(i - 1) + func(i));
  }
  return result;
}

template <typename TData, template <class> class TFunc>
void calc_next_approximation(
    const TFunc<TData> &func,
    const std::vector<TData> &x_data,
    const std::vector<TData> &data,
    std::vector<TData> &result) {
  result.resize(data.size());
  
  for (size_t i = 0; i < data.size(); ++i) {
    const TData &x = x_data[i];
    Integrable<TData, TFunc> integrable = Integrable<TData, TFunc>(x_data, data, func, x);
    result[i] = func.free_part(x) + calc_sum(integrable, x_data);
  }
}

template <typename TData, typename TFunc>
void calc_first_approximation(
    const TFunc &func,
    const std::vector<TData> &x_data,
    std::vector<TData> &result) {
  result.resize(x_data.size());
  for (size_t i = 0; i < x_data.size(); ++i) {
    const TData &x = x_data[i];
    result[i] = func.free_part(x);
  }
}

int main() {
  typedef double TData;

  TData x_0 = 0.0;
  TData x_1 = M_PI;
  std::vector<TData> x_data, data, tmp;

  make_x_data(x_0, x_1, 0.01, x_data);
  // method is convergent at lambda in [-0.63 .. 0.63] i.e. [-2/Pi .. 2/Pi]
  Function<TData> func(0.5);

  calc_first_approximation(func, x_data, data);
  for (size_t i = 0; i < 1e4; ++i) {
    calc_next_approximation(func, x_data, data, tmp);
    TData diff = major_diff(data, tmp);
    std::cout << "diff: " << diff << std::endl;
    data.swap(tmp);
    if (diff < 1e-6) { // stop when required precision 
      break;
    }
  }

  x_data.resize(101); //[0.0, 0.01, .. 1.0]
  std::ofstream os("outfile.txt");
  os.precision(6);
  print_data(os, x_data, data);
  os.close();
  return 0;
}
