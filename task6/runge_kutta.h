#pragma once

#include <cmath>
#include <vector>

/* == BEGIN: Math subrotines == */
template <typename TData>
TData pow2_3(const TData &a) {
  return a * std::sqrt(a);
}

template <typename TData>
void apply(std::vector<TData> &lhs, const std::vector<TData> &rhs, const TData &multiplier) {
  for (size_t i = 0; i < lhs.size(); ++i) {
    lhs[i] += multiplier * rhs[i];
  }
}

template <typename TData, typename TIterator, typename TRIterator>
void apply(TIterator it, TIterator end, TRIterator r_it, const TData &multiplier) {
  for (; it != end; ++it, ++r_it) {
    *it += multiplier * *r_it;
  }
}

template <typename TData, typename TIterator>
TData sqr_sum(TIterator it, TIterator end) {
  TData sum = 0.0;
  for (; it != end; ++it) {
    sum += *it * *it;
  }
  return sum;
}
/* == END: Math subrotines == */

template <typename TData, typename TFunc>
class RungeKutta {
public:
  RungeKutta(const TFunc& func) 
    : func_(func) {}
 
  void calc(const std::vector<TData> &data, const TData& time, std::vector<TData> &result, const TData &delta_t) const {
    std::vector<TData> d1;
    std::vector<TData> d2;
    std::vector<TData> d3;
    std::vector<TData> d4;

    func_.get_derivative(data, time, d1);
    std::vector<TData> data_tmp(data);
    apply(data_tmp, d1, delta_t * 0.5);
    
    func_.get_derivative(data_tmp, time + 0.5 * delta_t, d2);
    data_tmp = data;
    apply(data_tmp, d2, delta_t * 0.5);

    func_.get_derivative(data_tmp, time + 0.5 * delta_t, d3);
    data_tmp = data;
    apply(data_tmp, d3, delta_t);
    
    func_.get_derivative(data_tmp, time + delta_t, d4);

    result = data;
    apply(result, d1, delta_t / 6.0);
    apply(result, d2, delta_t / 3.0);
    apply(result, d3, delta_t / 3.0);
    apply(result, d4, delta_t / 6.0);
  }
private:
  const TFunc& func_;
};

template <typename TFunc>
RungeKutta<typename TFunc::DataType, TFunc> runge_kutta(const TFunc& func) {
  return RungeKutta<typename TFunc::DataType, TFunc>(func);
}

template <typename TData>
class TBaseLogger {
public:
  virtual void log(const TData &time, const std::vector<TData> &data) {};
};

template <typename TData, typename TIntegrator>
void solve(
    const TIntegrator& integrator,
    const std::vector<TData> &data,
    std::vector<TData> &result,
    const TData &t_0,
    const TData &t_1,
    size_t count,
    TBaseLogger<TData> *logger_ptr = NULL) {
  TData delta_t = (t_1 - t_0) / count;
  std::vector<TData> current_data(data), new_data;
  if (logger_ptr != NULL) {
    logger_ptr->log(t_0, current_data);
  }
  for (size_t i = 0; i < count; ++i) {
    TData time = t_0 + i * delta_t;
    integrator.calc(current_data, time, new_data, delta_t);
    current_data.swap(new_data);
    if (logger_ptr != NULL) {
      logger_ptr->log(time + delta_t, current_data);
    }
  }
  result = current_data;
}

template <typename TData>
TData calc_error(const std::vector<TData> &lhs, const std::vector<TData> &rhs) {
  TData sum = 0.0;
  for (size_t i = 0; i < lhs.size(); ++i) {
    sum += (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
  }
  return std::sqrt(sum);
}

