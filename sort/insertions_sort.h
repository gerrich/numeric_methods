#pragma once

template <typename TIter>
void insertion_sort(TIter begin, TIter end) {
  if (begin == end) {
    return;
  }
  TIter unsorted_start = begin;
  ++unsorted_start;
  size_t cnt = 0;
  for (; unsorted_start != end; ++unsorted_start, ++cnt) {
    TIter lower_bound = std::lower_bound(begin, unsorted_start, *unsorted_start);
    // shift the right half of sorted part
    for (; lower_bound != unsorted_start; ++lower_bound) {
      std::swap(*lower_bound, *unsorted_start);
    }
    if (cnt % 1000 == 0) {
      std::cerr << ".";
    }
  }
}


