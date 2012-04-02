#pragma once

#include <vector>
#include <stdint.h>
#include <cstddef>
#include <cmath>
#include <cstring>

#include <iostream>
#include <iomanip>

template <typename TDigit>
int log2(const TDigit& num) {
  int power = 0;
  for(; num >> power != 0; ++power) {
  }
  return power;
}

template <int LENGTH>
class precise {
public:
  typedef int32_t TDigit;
  enum {base_power = 13};
  enum {base = 0x01 << base_power};

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
 
  static void _shift_right(TDigit* data, int delta_power) {
    int index_shift = delta_power / base_power;
    int power_shift = delta_power % base_power;

    int new_index = LENGTH - 1;
    TDigit mask = ~(~TDigit(0) << power_shift);
    for (; new_index - index_shift >= 0; --new_index) {
      data[new_index] = data[new_index - index_shift] >> power_shift;
      if (new_index - index_shift > 0) {
        data[new_index] += (data[new_index - index_shift - 1] & mask) << (base_power - power_shift);
      }
    }
    for (; new_index >= 0; --new_index) {
      data[new_index] = 0;
    }
  }

  const precise& add(const precise& rhs, int operation_sign) {
    if (power_ < rhs.power_) {
      _shift_right(data_, rhs.power_ - power_);
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

      if (data_[0] >= base) {
        int back_shift = log2(data_[0] >> base_power);
        _shift_right(data_, back_shift);
        power_ += back_shift;
      } else {
        int index = 0;
        for (; index < LENGTH && data_[index] == 0; ++index) {
        }
        int start_power = log2(data_[index]);
        int power_shift = base_power - start_power;
        TDigit mask = ~((~TDigit(0)) << start_power);
        int new_index = 0;
        for (; index < LENGTH; ++index, ++new_index) {
          data_[new_index]  = ((data_[index] & mask) << power_shift);
          if (index + 1 < LENGTH) {
            data_[new_index] += (data_[index + 1] >> start_power);
          }
        }
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
      int back_shift = log2(target_data[0] >> base_power);
      TDigit mask = ~((~TDigit(0)) << back_shift);
      for (int index = 0; index < LENGTH; ++index) {
        target_data[index + 1] += (target_data[index] & mask) << base_power;
        target_data[index] = target_data[index] >> back_shift;
      }
      power_ += back_shift;
    }
    int i = 0;
    for (; i < LENGTH && data_[i] == 0; ++i) {}
    memcpy(data_, &target_data[i], LENGTH * sizeof(TDigit));
    power_ -= base_power * i;

    return *this;
  }
 
  /* compare two digit arrays */ 
  static bool _cmp_ge(const TDigit* lhs, const TDigit* rhs) {
    for (size_t i = 0; i < LENGTH; ++i) {
      if (lhs[i] > rhs[i]) return true;
      if (lhs[i] < rhs[i]) return false;
    }
    return true;
  }

  static bool _substract(TDigit* lhs, const TDigit* rhs) {
    for (int i = LENGTH - 1; i >=0; --i) {
      lhs[i] -= rhs[i];
      if (lhs[i] < 0) {
        lhs[i - 1] -= 1;
        lhs[i] += base;
      }
    }
  }

  const precise& operator /= (const precise& rhs) {
    TDigit target[LENGTH];
    memset(target, 0, LENGTH * sizeof(TDigit));
    
    TDigit rcopy[LENGTH];
    memcpy(rcopy, rhs.data_, LENGTH * sizeof(TDigit));

    for (size_t index = 0; index < LENGTH * base_power; ++index) {
      size_t digit_index = index / base_power;
      size_t bit_index = base_power - 1 - index % base_power;

      if (_cmp_ge(data_, rcopy)) {
        _substract(data_, rcopy);
        target[digit_index] = target[digit_index] | (0x01 << bit_index);
      }

      _shift_right(rcopy, 1);
    }

    memcpy(data_, target, LENGTH * sizeof(TDigit));
    power_ -= rhs.power_ - 1;
    sign_ *= rhs.sign_;
  }

  operator double() {
    double mantissa = 0;
    for (int index = LENGTH - 1; index >= 0; --index, mantissa /= double(base)) {
      mantissa += static_cast<double>(data_[index]);
    }
    return ldexp(sign_ * mantissa, power_);
  }

  void print(std::ostream& os) const;

private:
  int sign_;
  int power_;
  TDigit data_[LENGTH];
};

/* hi precision integer number class for x2 base -> x10 base conversion */
template <int LENGTH = 10, int BASE = 10, int BASE_POWER = 1>
class bigint {
public:
  typedef int32_t TDigit;
  enum {base = BASE};
  enum {length = LENGTH};
  enum {base_power = BASE_POWER};

  bigint() {
    memset(data_, 0, sizeof(TDigit) * LENGTH);
  }

  bigint(int data) {
    memset(data_, 0, sizeof(TDigit) * LENGTH);
    for (size_t i = 0; i < LENGTH; ++i) {
      data_[i] = data % base;
      data /= base;
    }
  }

  const bigint& operator = (const TDigit &rhs) {
    memset(data_, 0, sizeof(TDigit) * LENGTH);
    data_[0] = rhs;
    for (size_t i = 1; i < LENGTH; ++i) {
      data_[i] = data_[i - 1] / base;
      data_[i - 1] %= base;
    }
    return *this; 
  }

  const bigint& operator += (const TDigit &rhs) {
    data_[0] += rhs;
    for (size_t i = 0; i + 1 < LENGTH; ++i) {
      data_[i+1] += data_[i] / base;
      data_[i] %= base;
    }
    return *this;
  }
  
  const bigint& operator += (const bigint &rhs) {
    for (size_t i = 0; i < LENGTH; ++i) {
      data_[i] += rhs.data_[i];
      if (i + 1 < LENGTH) {
        data_[i+1] += data_[i] / base;
        data_[i] %= base;
      }
    }
    return *this;
  }

  const bigint& multiply(const TDigit &rhs, int &power) {
    TDigit buffer[LENGTH * 2];
    memset(buffer, 0, 2 * LENGTH * sizeof(TDigit));
    for (size_t i = 0; i < LENGTH; ++i) {
      buffer[i] = data_[i] * rhs;
    }
    for (size_t i = 0; i + 1 < 2 * LENGTH; ++i) {
      buffer[i+1] += buffer[i] / base;
      buffer[i] %= base;
    }
    int index = 2 * LENGTH - 1;
    for (; index >= LENGTH && buffer[index] == 0; --index) {}
    memcpy(data_, &buffer[index + 1 - LENGTH], LENGTH * sizeof(TDigit));
    power = index + 1 - LENGTH;
    return *this;
  }

  const TDigit* get_data() const {
    return data_;
  }
  
  TDigit* get_data() {
    return data_;
  }

private:
  TDigit data_[LENGTH];
};

/* returns base-power of result */
template <typename TBigInt>
int multiply(const TBigInt &lhs, const TBigInt &rhs, TBigInt &target) {
  typename TBigInt::TDigit target_data[TBigInt::length * 2]; 
  memset(target_data, 0, sizeof(typename TBigInt::TDigit) * 2 * TBigInt::length);

  for (size_t i = 0; i < TBigInt::length; ++i) {
    for (size_t j = 0; j < TBigInt::length; ++j) {
      target_data[i + j] += lhs.get_data()[i] * rhs.get_data()[j];
    }
  }
  for (size_t i = 0; i < 2 * TBigInt::length - 1; ++i) {
    target_data[i + 1] += target_data[i] / TBigInt::base;
    target_data[i] %= TBigInt::base;
  }
  int index = 2 * TBigInt::length - 1;
  for (; index >= TBigInt::length && target_data[index] == 0; --index) {}
  if (index >= TBigInt::length) {
    memcpy(target.get_data(), &target_data[index + 1 - TBigInt::length], sizeof(typename TBigInt::TDigit) * TBigInt::length);
    return index + 1 - TBigInt::length;
  } else {
    memcpy(target.get_data(), target_data, sizeof(typename TBigInt::TDigit) * TBigInt::length);
    return 0;
  }
}

template <typename TBigInt>
void round_bigint(TBigInt& number, int length) {
  int i = 0;
  for (; i < length; ++i) {
    if (number.get_data()[i] >= TBigInt::base) {
      number.get_data()[i + 1] += number.get_data()[i] / TBigInt::base;
      number.get_data()[i] %= TBigInt::base;
    }
    if (number.get_data()[i] * 2 >= TBigInt::base) {
      number.get_data()[i] = 0;
      number.get_data()[i + 1] += 1;
    } else {
      number.get_data()[i] = 0;
    }
  }
  for (; i + 1 < TBigInt::length; ++i) {
    if (number.get_data()[i] >= TBigInt::base) {
      number.get_data()[i + 1] += number.get_data()[i] / TBigInt::base;
      number.get_data()[i] %= TBigInt::base;
    }
  }
}

template <typename TBigInt>
int print_bigint(const TBigInt& number, std::ostream& os, bool print_dot = false, int round = 0) {
  int i = TBigInt::length - 1;
  for (; i > round && number.get_data()[i] == 0; --i) {
  }
  int start_index = i;
  os << number.get_data()[i];
  --i;
  if (print_dot) {
    os << '.';
  }
  for (; i >= round; --i) {
    os << std::setfill('0') << std::setw(TBigInt::base_power) << number.get_data()[i];
  }
  return start_index;
}

template <int LENGTH>
void precise<LENGTH>::print(std::ostream& os) const {
  static const int dec_base = 10;
  static const int dec_length = LENGTH * 4;
  static const int dec_base_power = 1;
  typedef bigint<dec_length, dec_base, dec_base_power> TLongDec;

  TLongDec buffer;
  int dec_power = 0;
  TDigit multiplier = 1;
  for (int index = 0; index < LENGTH; ++index) {
    if (index > 0) {
      int operation_power = 0;
      buffer.multiply(base, operation_power);
      dec_power += operation_power;
      for (int i = 0; multiplier < base && i < operation_power; ++i) {
        multiplier *= dec_base;
      }
    }
//    std::cout << "[" << data_[index] << "]";
    buffer += data_[index] / multiplier;
  }
//  std::cout << std::endl;

  TLongDec exp_buffer(1);
  int exp_dec_power = 0;

  int base_power_needed = power_ - LENGTH * base_power; 

  TLongDec exp_multiplier(2);
  int exp_dec_multiplier_power = 0;
  
  if (base_power_needed < 0) {
    base_power_needed *= -1;
    exp_multiplier = 5;
    exp_dec_multiplier_power = -1;
  }
  if (base_power_needed > 0) {
    for (int current_base_power = 1; current_base_power <= base_power_needed; current_base_power *= 2) {
      if (base_power_needed & current_base_power) {
        int add_dec_power = multiply(exp_multiplier, exp_buffer, exp_buffer);
        exp_dec_power = exp_dec_power + exp_dec_multiplier_power + add_dec_power;
      }

      int add_dec_power = multiply(exp_multiplier, exp_multiplier, exp_multiplier);
      exp_dec_multiplier_power = exp_dec_multiplier_power * 2 + add_dec_power;
    }
    int add_dec_power = multiply(buffer, exp_buffer, buffer);
    dec_power += add_dec_power + exp_dec_power;
  }

//  round_bigint(buffer, dec_length / 5);
  if (sign_ < 0) {
    os << "-";
  }
  int start_index = print_bigint(buffer, os, true);//, dec_length / 5);
  os << "e" << dec_power + start_index; 
}

