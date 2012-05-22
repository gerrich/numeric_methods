#pragma once
#include <vector>
#include <iostream>

template <typename TData>
void make_x_data(const TData &x_0, const TData &x_1, const TData &d_x, std::vector<TData> &x_data) {
  for (TData x = x_0; x < x_1; x += d_x) {
    x_data.push_back(x);
  }
  x_data.push_back(x_1);
}

template <typename TData>
void print_data(std::ostream &os, const std::vector<TData> &x_data, const std::vector<TData> &data) {
  for (size_t i = 0; i < x_data.size(); ++i) {
    os << x_data[i] << " " << data[i] << std::endl;
  }
}

template <typename TData>
TData major_diff(const std::vector<TData> &l_data, const std::vector<TData> &r_data) {
  TData max_diff = 0.0;
  for (size_t i = 0; i < l_data.size(); ++i) {
    TData diff = std::abs(l_data[i] - r_data[i]);
    if (diff > max_diff) {
      max_diff = diff;
    } 
  }
  return max_diff;
}


