#include <cmath>
#include <vector>

#include <iostream>
#include <fstream>

#include "runge_kutta.h"

template <typename TData>
class Function {
public:
  typedef TData DataType;
  void get_derivative(const std::vector<TData> &data, const TData &time, std::vector<TData> &result) const {
    //data[0] -> y
    //data[1] -> dy/dt

    result.resize(data.size());
    result[0] = data[1];
    result[1] = exp(time) + 2.0 * sin(data[0]);
  }
  
};

template <typename TData>
class OutFileLogger : public TBaseLogger<TData> {
public:
  OutFileLogger(std::ostream &os) 
    : os_(os) {
  }

  void log(const TData &time, const std::vector<TData> &data) {
    os_ << time << " " << data[0] << std::endl;
  }
private:
  std::ostream &os_;
};

template <typename TData>
class Shooter {
public:
  Shooter(TBaseLogger<TData> *logger_ptr = NULL)
    : logger_ptr_(logger_ptr) {
  }

  TData calc(const TData &x) const {
    TData t_0 = 0.0;
    TData t_1 = 1.0;
    size_t count = 20;
    std::vector<TData> data(2, 0.0);
    data[0] = 1.0;
    data[1] = x;

    std::vector<TData> result;

    solve(runge_kutta(Function<TData>()), data, result, t_0, t_1, count, logger_ptr_);
    return result[0];
  }

private:
  TBaseLogger<TData>* logger_ptr_;
};

template <typename TData>
bool guess(TData x_0, TData x_1, const TData &y, Shooter<TData> &shooter, TData &result_x, TData &accuracy) {
  
  TData y_0 = shooter.calc(x_0);
  TData y_1 = shooter.calc(x_1);

  std::cout << "y_0: " << y_0 << std::endl;
  std::cout << "y_1: " << y_1 << std::endl;

  // assume y is between y_0 and y_1
  if ((y_0 - y) * (y_1 - y) > 0) {
    return false;
  }

  for (size_t i = 0; i < 1e4; ++i) {
    if (std::abs(y_0 - y) < accuracy) {
      result_x = x_0;
      return true;
    }
    if (std::abs(y_1 - y) < accuracy) {
      result_x = x_1;
      return true;
    }

    result_x = x_0 + (x_1 - x_0) / (y_1 - y_0) * (y - y_0);
    TData new_y = shooter.calc(result_x);
    

    if ((new_y - y) * (y_0 - y) > 0) {
      std::cout << "y_0: " << new_y << std::endl;
      y_0 = new_y;
      x_0 = result_x;
    } else {
      std::cout << "y_1: " << new_y << std::endl;
      y_1 = new_y;
      x_1 = result_x;
    }
  }
  return false;
}

int main() {
  typedef double TData;
  
  TData dy_dx;
  TData dy_dx_0 = 0.0;
  TData dy_dx_1 = 2.0;
  TData y_1 = 3.0;
  TData accuracy = 1e-7;

  Shooter<TData> silent_shooter;
  if (guess(dy_dx_0, dy_dx_1, y_1, silent_shooter, dy_dx, accuracy)) {
    std::ofstream os("outfile.txt");
    os.precision(10);
    OutFileLogger<TData> logger(os);
    
    Shooter<TData>(&logger).calc(dy_dx);
  } else {
    std::cout << "Failed to find initial conditions" << std::endl;
  }

  return 0;
}
