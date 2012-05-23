#include <cmath>
#include <vector>

#include <iostream>
#include <fstream>

#include "runge_kutta.h"

const double m_1 = 1.0;
const double m_2 = 2.0;
const double m_3 = 3.0;

template <typename TData>
class Function {
public:
  typedef TData DataType;
  void get_derivative(const std::vector<TData> &data, const TData &time, std::vector<TData> &result) const {
    std::vector<TData> diff = data;
    apply(diff.begin() + 0, diff.begin() + 3, data.begin() + 3, -1.0); //diff_1 = data_1 - data_2
    apply(diff.begin() + 3, diff.begin() + 6, data.begin() + 6, -1.0); //diff_2 = data_2 - data_3
    apply(diff.begin() + 6, diff.begin() + 9, data.begin() + 0, -1.0); //diff_3 = data_3 - data_1

    TData len12 = pow2_3(sqr_sum<TData>(diff.begin() + 0, diff.begin() + 3));
    TData len23 = pow2_3(sqr_sum<TData>(diff.begin() + 3, diff.begin() + 6));
    TData len31 = pow2_3(sqr_sum<TData>(diff.begin() + 6, diff.begin() + 9));

    result.assign(data.size(), 0.0);
    
    apply(result.begin(), result.begin() + 9, data.begin() + 9, 1.0);  // first derivatives are stored at indices [9..17]

    apply(result.begin() +  9, result.begin() + 12, diff.begin() + 0, m_2 / len12);
    apply(result.begin() +  9, result.begin() + 12, diff.begin() + 6, -m_3 / len31);

    apply(result.begin() + 12, result.begin() + 15, diff.begin() + 3, m_3 / len23);
    apply(result.begin() + 12, result.begin() + 15, diff.begin() + 0, -m_1 / len12);
    
    apply(result.begin() + 15, result.begin() + 18, diff.begin() + 6, m_1 / len31);
    apply(result.begin() + 15, result.begin() + 18, diff.begin() + 3, -m_2 / len23);
  }

  // Energy remain the same in an enclosed system. Compare energy before and after evaluation
  TData get_energy(const std::vector<TData> &data) {
    TData energy = 0.0;
    // kinetic
    energy += 0.5 * m_1 * sqr_sum<TData>(data.begin() +  9, data.begin() + 12);
    energy += 0.5 * m_2 * sqr_sum<TData>(data.begin() + 12, data.begin() + 15);
    energy += 0.5 * m_3 * sqr_sum<TData>(data.begin() + 15, data.begin() + 18);
    
    // anti-gravity ;-)
    std::vector<TData> diff = data;
    apply(diff.begin() + 0, diff.begin() + 3, data.begin() + 3, -1.0); //diff_1 = data_1 - data_2
    apply(diff.begin() + 3, diff.begin() + 6, data.begin() + 6, -1.0); //diff_2 = data_2 - data_3
    apply(diff.begin() + 6, diff.begin() + 9, data.begin() + 0, -1.0); //diff_3 = data_3 - data_1
    
    energy += m_1 * m_2 / std::sqrt(sqr_sum<TData>(diff.begin() + 0, diff.begin() + 3));
    energy += m_2 * m_3 / std::sqrt(sqr_sum<TData>(diff.begin() + 3, diff.begin() + 6));
    energy += m_3 * m_1 / std::sqrt(sqr_sum<TData>(diff.begin() + 6, diff.begin() + 9));

    return energy;
  }
};

int main() {
  typedef double TData;

  std::vector<TData> data(18, 0.0);
  data[0] = 1.0;
  data[4] = 1.0; //0.99; // slightly change initial data to evaluate 2nd subtask
  data[8] = 1.0;
  
  data[10] = 1.0;
  data[14] = 1.0;
  data[17] = 1.0;
  
  TData t_0 = 0.0;
  TData t_1 = 100.0;
  size_t count = 100000;

  TData e_0 = Function<TData>().get_energy(data);

  std::vector<TData> result;
  solve(runge_kutta(Function<TData>()), data, result, t_0, t_1, count);
  
  std::vector<TData> second_result;
  solve(runge_kutta(Function<TData>()), data, second_result, t_0, t_1, count / 2);

  std::cout.precision(16);

  // accuracy is estimated as difference between solutions with various number of steps
  std::cout << "error: " << calc_error(result, second_result) << std::endl;

  // print position of 1st point
  std::cout << "data: " << result[0] << " " << result[1] << " " << result[2] << std::endl;

  TData e_1 = Function<TData>().get_energy(result);
  TData e_2 = Function<TData>().get_energy(second_result);

  std::cout << "e0: " << e_0 << std::endl;
  std::cout << "e1: " << e_1 << std::endl;
  std::cout << "e2: " << e_2 << std::endl;
  
  std::ofstream os("outfile.txt");
  os.precision(16);
  os << result[0] << " " << result[1] << " " << result[2] << std::endl;
  os.close();

  return 0;
}
