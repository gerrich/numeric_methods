#pragma once

#include <iostream>

class Complex {
public:
  explicit Complex(const double& re)
    : re_(re)
    , im_(0.0) {
  }
  
  Complex(const double& re, const double &im)
    : re_(re)
    , im_(im) {
  }

  Complex(const Complex &rhs)
    : re_(rhs.re_)
    , im_(rhs.im_) {
  }

  const Complex& operator += (const Complex &rhs) {
    re_ += rhs.re_;
    im_ += rhs.im_;
    return *this;
  }

  const Complex& operator -= (const Complex &rhs) {
    re_ -= rhs.re_;
    im_ -= rhs.im_;
    return *this;
  }

  const Complex& operator *= (const Complex &rhs) {
    double new_re = re_ * rhs.re_ - im_ * rhs.im_;
    im_ = re_ * rhs.im_ + im_ * rhs.re_;
    re_ = new_re;
    return *this;
  }

  const Complex operator * (const Complex &rhs) const {
    return Complex(re_ * rhs.re_ - im_ * rhs.im_, re_ * rhs.im_ + im_ * rhs.re_);
  }
  
  const Complex operator / (const Complex &rhs) const {
    double mod = rhs.re_ * rhs.re_ + rhs.im_ * rhs.im_;
    return Complex(
      (re_ * rhs.re_ + im_ * rhs.im_) / mod,
      (-re_ * rhs.im_ + im_ * rhs.re_) / mod);
  }

  const Complex& operator /= (const Complex &rhs) {
    *this = *this / rhs;
    return *this;
  }

  const double& re() const {
    return re_;
  }

  const double& im() const {
    return im_;
  }

private:
  double re_;
  double im_;
};

std::ostream& operator << (std::ostream& os, const Complex& data) {
  os << data.re() << "+i*" << data.im();
}

