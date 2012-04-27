#pragma once

#include <vector>
#include <exception>

#include <stddef.h>
#include <stdint.h>

#include <iostream>
#include <cmath>

using std::abs;

class math_ex : public std::exception {
};


template <typename vector_t, typename data_t>
void add(vector_t &lhs, const vector_t &rhs, const data_t &factor) {
  if (lhs.size() != rhs.size()) {
    throw math_ex(); 
  }
  for (size_t index = 0 ;index < lhs.size(); ++index) {
    lhs[index] += factor * rhs[index];
  }
}

template <typename data_t>
void solve(std::vector< std::vector<data_t> > &matrix, std::vector<data_t> &y, std::vector<data_t> &x) {
  if (matrix.size() != y.size()) {
    throw math_ex(); 
  }

  for (size_t index = 0; index < matrix.size(); ++index) {
    size_t best_j = index;
    size_t max_val = 0;
    for (size_t j = index; j < matrix.size(); ++j) {
      double val = abs(matrix[j][index]);
      if (val > max_val) {
        max_val = val;
        best_j = j;
      }
    }
    if (best_j != index) {
      matrix[index].swap(matrix[best_j]);
      std::swap(y[index], y[best_j]);
    }
    for (size_t j = 0; j < matrix.size(); ++j) { 
      if (index == j) continue;
      data_t factor = -matrix[j][index] / matrix[index][index];
      add(matrix[j], matrix[index], factor);
      matrix[j][index] = 0.0;
      y[j] += y[index] * factor;
    }
  }

  x.resize(matrix.size());
  for (size_t index = 0; index < matrix.size(); ++index) {
    x[index] = y[index] / matrix[index][index];
  }
}
