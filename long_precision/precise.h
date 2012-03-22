#pragma once

#include <vector>
#include <stdint.h>
#include <cstddef>
#include <cmath>
#include <cstring>

template <typename TDigit>
int log2(const TDigit& num) {
  int power = 0;
  for(; num >> power != 0; ++power) {
  }
  return power;
}

template <int LENGTH>
class precise {
  typedef int32_t TDigit;
  static const TDigit base = 8*1024;
  static const int base_power = 13;
public:
  precise()
    : sign_(1) 
    , power_(0) {
    for (size_t index = 0; index < LENGTH; ++index) {
      data_[index] = 0;
    }  
    data_[LENGTH -1] = 1;
  }
  precise(const double& value) {
    double mantissa = frexp(value, &power_);
    if (mantissa < 0) {
      mantissa = - mantissa;
      sign_ = -1;
    } else {
      sign_ = 1;
    }
    double zero = 0;
    mantissa *= base;
    for (size_t index = 0; index < LENGTH; ++index, mantissa *= base) {
      double integer_part;
      mantissa = modf(mantissa, &integer_part);
      data_[index] = static_cast<TDigit>(integer_part);
    }
    // rize last bit 
    data_[LENGTH - 1] = std::max(data_[LENGTH - 1], TDigit(1));
  }
 
  const precise& add(const precise& rhs, int operation_sign) {
    if (power_ < rhs.power_) {
      int index_shift = (rhs.power_ - power_) / base_power;
      int power_shift = (rhs.power_ - power_) % base_power;
      
      int new_index = LENGTH - 1;
      TDigit mask = ~(~TDigit(0) << power_shift);
      for (; new_index - index_shift >= 0; --new_index) {
        data_[new_index] = data_[new_index - index_shift] >> power_shift;
        if (new_index - index_shift > 0) {
          data_[new_index] += (data_[new_index - index_shift - 1] & mask) << (base_power - power_shift);
        }
      }
      for (; new_index >= 0; --new_index) {
        data_[new_index] = 0;
      }
      power_ = rhs.power_;
    }
    if (/* true */power_ >= rhs.power_) {
      int power_shift = base_power - (power_ - rhs.power_) % base_power;
      int index_shift = (power_ - rhs.power_) / base_power + (power_shift > 0 ? 1 : 0);

      int sign = operation_sign * sign_ * rhs.sign_;
      if (power_shift > 0) {
        data_[LENGTH - 1] += sign * (rhs.data_[LENGTH - index_shift] >> power_shift);
      }
      for (int index = LENGTH - 1; index >= index_shift; --index) {
        data_[index] += sign * (rhs.data_[index - index_shift] << power_shift);
        if (index > 0) {
          data_[index - 1] += data_[index] / base;
          data_[index] %= base;
          if (data_[index] < 0) {
            data_[index - 1] -= 1;
            data_[index] += base;
          }
        }
      }
      for (int index = 0; index < LENGTH; ++index) {
        std::cout << "(" << data_[index] << ")";
      }
      std::cout << std::endl;

      // fix sign
      if (data_[0] < 0) {
        sign_ = -sign_;

        for (int index = LENGTH - 1; index > 0; --index) {
          if (data_[index] != 0) {
            data_[index] = base - data_[index];
            data_[index - 1] += 1;
          }
        }
        data_[0] = -data_[0];
      }
      for (int index = 0; index < LENGTH; ++index) {
        std::cout << "(" << data_[index] << ")";
      }
      std::cout << std::endl;

      std::cout << data_[0] << ":" << base << " ";
      if (data_[0] >= base) {
        std::cout << "2.1.." << std::endl;
        int back_shift = log2(data_[0] >> base_power);
        TDigit mask = ~((~TDigit(0)) << back_shift);
        for (int index = 0; index < LENGTH; ++index) {
          data_[index + 1] += (data_[index] & mask) << base_power;
          data_[index] = data_[index] >> back_shift;
        }
        power_ += back_shift;
      } else {
        std::cout << "2.2.." << std::endl;
        int index = 0;
        for (; index < LENGTH && data_[index] == 0; ++index) {
        }
        int start_power = log2(data_[index]);
        std::cout << start_power << std::endl;
        int power_shift = base_power - start_power;
        TDigit mask = ~((~TDigit(0)) << start_power);
        int new_index = 0;
        for (; index < LENGTH; ++index, ++new_index) {
          data_[new_index]  = ((data_[index] & mask) << power_shift);
          if (index + 1 < LENGTH) {
            data_[new_index] += (data_[index + 1] >> start_power);
          }
          std::cout << "[" << data_[new_index] << "]";
        }
        std::cout << std::endl;
        power_ -= power_shift;
        power_ -= (index - new_index) * base_power;
      }
    }

    return *this;
  }
  const precise& operator +=(const precise& rhs) {
    return add(rhs, 1);
  }
  const precise& operator -=(const precise& rhs) {
    return add(rhs, -1);
  }
  const precise& operator *=(const precise& rhs) {
    sign_ *= rhs.sign_;
    power_ += rhs.power_;

    TDigit target_data[2 * LENGTH - 1];
    memset(&target_data, 0, (2 * LENGTH - 1) * sizeof(TDigit));

    for (int first_index = 0; first_index < LENGTH; ++first_index) {
      for (int second_index = 0; second_index < LENGTH; ++second_index) {
        target_data[1 + first_index + second_index] += data_[first_index] * rhs.data_[second_index];
      } 
    }
    for (int index = LENGTH - 1; index > 0; --index) {
      target_data[index - 1] += target_data[index] / base;
      target_data[index] %= base; 
    }
    if (target_data[0] >= base) {
      std::cout << "2.1.." << std::endl;
      int back_shift = log2(target_data[0] >> base_power);
      TDigit mask = ~((~TDigit(0)) << back_shift);
      for (int index = 0; index < LENGTH; ++index) {
        target_data[index + 1] += (target_data[index] & mask) << base_power;
        target_data[index] = target_data[index] >> back_shift;
      }
      power_ += back_shift;
    }
    memcpy(data_, target_data, LENGTH * sizeof(TDigit));

    return *this;
  }
  const precise& operator /(const precise& rhs) {}

  operator double() {
    double mantissa = 0;
    for (int index = LENGTH - 1; index >= 0; --index, mantissa /= double(base)) {
      mantissa += static_cast<double>(data_[index]);
    }
    return ldexp(sign_ * mantissa, power_);
  }

  void print() {
    int dec_base = 10000;
    TDigit buffer[LENGTH];
    memset(buffer, 0, LENGTH * sizeof(TDigit));

    for (int index = 0; index < LENGTH; ++index) {
      if (index > 0) {
        for (int i = 0; i < LENGTH; ++i) {
          buffer[i] *= base;
        }
      }
      buffer[LENGTH - 1] += data_[index];
      for (int i = LENGTH - 1; i > 0; --i) {
        buffer[i - 1] += buffer[i] / dec_base;
        buffer[i] %= dec_base;
      }
    }
  }

private:
  int sign_;
  int power_;
  TDigit data_[LENGTH];
};

