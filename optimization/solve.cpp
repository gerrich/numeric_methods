#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

template <typename TData>
TData pow2(const TData &a) {
  return a * a;
}
template <typename TData>
TData pow3(const TData &a) {
  return a * a * a;
}
template <typename TData>
TData pow4(const TData &a) {
  return pow2(pow2(a));
}

double alpha_ = 1.4847620;
double beta_ = 1.3631090;
double gamma_ = 1.9324320;
double delta_  = 1.9879150;
double S_ = 1;

double dr_ = 0.025;

template <typename TData>
class Sample {
public:
  static void value(const std::vector<TData> &data, TData &result) {
    result = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
      result += pow2(i * data[i]);
    }
  }
  static void derivative(const std::vector<TData> &data, std::vector<TData> &result) {
    result.assign(data.size(), 0.0);
    for (size_t i = 0; i < data.size(); ++i) {
      result[i] = pow2(i) * 2 * data[i];
    }
  }
};

template <typename TData>
class Function {
public:
  static void value(const std::vector<TData> &data, TData &result) {
    TData a;
    TData b;
    top(data, a);
    bottom(data, b);
    
    result = a / b;
  }

  static void top(const std::vector<TData> &data, TData &result) {
    result = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
      result += alpha_ * pow2(sin(data[i])) * dr_;
      result += beta_ * pow4(sin(data[i])) * dr_ / pow2(0.01 + 0.025 * i);
    }
    
    for (size_t i = 0; i + 1 < data.size(); ++i) {
      result += gamma_ * pow2(0.01 + 0.025 * i) / dr_ * pow2(data[i+1] - data[i]); 
      result += delta_ * pow4(sin(data[i])) * pow2(data[i+1] - data[i]) / dr_;
    }
  }

  static void bottom(const std::vector<TData> &data, TData &devider) {
    devider = 1 + S_ * (S_ + 1);
    for (size_t i = 0; i < data.size(); ++i) {
      devider += pow4(sin(data[i])) * dr_;
    }
  }

  static void derivative(const std::vector<TData> &data, std::vector<TData> &result) {
    result.assign(data.size(), 0.0);

    TData a;
    TData b;
    
    top(data, a);
    bottom(data, b);

    for (size_t i = 0; i < data.size(); ++i) {
      result[i] += alpha_ * 2.0 * sin(data[i]) * cos(data[i]) * dr_;
      result[i] += beta_ * 4.0 * pow3(sin(data[i])) * cos(data[i]) * dr_ / pow2(0.01 + 0.025 * i);
    }
    for (size_t i = 0; i + 1 < data.size(); ++i) {
      result[i+1] += gamma_ * pow2(0.01 + 0.025 * i) / dr_ * 2.0 * (data[i+1] - data[i]); 
      result[i] -= gamma_ * pow2(0.01 + 0.025 * i) / dr_ * 2.0 * (data[i+1] - data[i]); 

      result[i+1] += delta_ * pow4(sin(data[i])) * 2.0 * (data[i+1] - data[i]) / dr_;
      result[i] -= delta_ * pow4(sin(data[i])) * 2.0 * (data[i+1] - data[i]) / dr_;
      result[i] += delta_ * 4.0 * pow3(sin(data[i])) * cos(data[i]) * pow2(data[i+1] - data[i]) / dr_;
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
      result[i] /= b;
    }

    for (size_t i = 0; i < data.size(); ++i) {
      result[i] -= a / pow2(b) * 4.0 * pow3(sin(data[i])) * cos(data[i]) * dr_;
    }
  }
};


template <typename TData>
TData max_abs(const std::vector<TData> &data) {
  TData value = 0;
  for (size_t i = 0; i < data.size(); ++i) {
    if (value < std::abs(data[i])) {
      value = std::abs(data[i]);
    }
  }
  return value;
}

template <typename TData>
void apply(std::vector<TData>& data, const std::vector<TData>& increment, TData multiplier) {
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] += multiplier * increment[i];
  }
}

template <typename TData>
TData cosine(const std::vector<TData>& lhs, const std::vector<TData> &rhs) {
  TData result = 0;
  TData lsum = 0;
  TData rsum = 0;
  for (size_t i = 0; i < lhs.size(); ++i) {
    result += lhs[i] * rhs[i];
    lsum += lhs[i] * lhs[i];
    lsum += rhs[i] * rhs[i];
  }
  TData devider = lsum * rsum;
  if (devider == 0) {
    return 0;
  } else {
    return result / std::sqrt(devider);
  }
}

template <typename TData, template <typename TData> class TFunc> 
void solve(std::vector<TData> &data) {
  // TODO
  std::vector<TData> velocity(data.size(), 0.0);
  std::vector<TData> derivative;
  TData value;

  TData inertia_factor = 1e-6;
  TData null_derivative = 1e-10;

  TData prev_iteration_value;
  TData prev_iteration_max_derivative;
  std::vector<TData> prev_iteration_data;

  int count = 0;
  int delta_ok_counter = 0;
  while (true) {
    TFunc<TData>::derivative(data, derivative);
   
    TData value;
    TFunc<TData>::value(data, value);

    apply<TData>(data, velocity, 1.0);
    apply<TData>(velocity, derivative, -inertia_factor);
    apply<TData>(velocity, velocity, -0.01 - 1.0 * cosine(velocity, derivative));

    TData max_derivative = max_abs(derivative);
    if (count % 1000 == 0) {
      std::cout << "F(x) " << value << "\td_max: " << max_derivative << "\tIF: " << inertia_factor  << "\tcount: " << count << std::endl;
     
      /* 
      if (count > 0 and prev_iteration_value - value < 0.0) {
        // roll back
        data = prev_iteration_data;
        velocity.assign(data.size(), 0.0);
        inertia_factor /= 2;

      } else 
      */{
        prev_iteration_value = value;
        prev_iteration_data = data;
        if (count > 0) {
          if (1.01 * max_derivative - prev_iteration_max_derivative < 0) {
            delta_ok_counter += 1;
          } else if (max_derivative - prev_iteration_max_derivative > 0) {
            delta_ok_counter = 0;
            inertia_factor /= 1.1;
          }
          if (delta_ok_counter > 10) {
            delta_ok_counter -= 5;
            inertia_factor *= 1.1;
          }
        }
        prev_iteration_max_derivative = max_derivative;
      }
    }
    if (max_derivative < null_derivative) {
      break;
    }
    ++count;
  }
  std::cout << "iter count: " << count << std::endl;
}

template <typename TData>
void print_data(std::ostream &os, std::vector<TData>& data) {
  std::sort(data.begin(), data.end());
  os.precision(16); 
  for (int i = 0; i < data.size(); ++i) {
    if (i > 0) {
      os << std::endl;
    }
    os << data[i];
  }
}

int main() {
  typedef double TData;
  std::vector<TData> data(401, 0.0);
  data[0] = M_PI;
  
  std::cout.precision(16);
  
  solve<TData, Function>(data);
  std::ofstream os("outfile.txt");
  print_data(os, data);

  return 0;
}
