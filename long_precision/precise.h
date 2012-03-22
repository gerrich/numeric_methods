#pragma once

#include <vector>

template <int LENGTH>
class precise {
  typedef int32_t TDigit;
  const TDigit base = 2**13;
public:
  precise() {}
  precise(const double& value) {}
 
  const precise& operator +(const precise& rhs) {}
  const precise& operator -(const precise& rhs) {}
  const precise& operator *(const precise& rhs) {}
  const precise& operator /(const precise& rhs) {}

  double operator double() {}

private:
  TDigit sign_;
  TDigit power_;
  TDigit data_[LENGTH];
};

