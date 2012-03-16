#pragma once

template <typename TIter>
void qsort(TIter begin, TIter end) {
  bool direction = true;
  TIter front = begin;
  TIter back = end;
  --back; // single step back
  size_t left_size = 0;
  size_t right_size = 1;
  for (; front != back; ) {
    if (*front < *back) {
    } else {
      std::swap(*front, *back);
      direction = not direction;
    }
    if (direction) {
      ++front;
      ++left_size;
    } else {
      --back;
      ++right_size;
    }
  }
  if (left_size > 1) {
    qsort(begin, front);
  }
  if (right_size > 1) {
    qsort(front, end);
  }
}
