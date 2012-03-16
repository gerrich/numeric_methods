#include <vector>

#include <iostream>
#include <fstream>


typedef unsigned long ulong;

const size_t DATA_SIZE = 1e7;
const ulong ALPHA = 42;
const ulong FACTOR_B = 1664525;
const ulong FACTOR_A = 1013904223;
const ulong INITIAL_FACTOR = 111;
const size_t STEP_SIZE = 100000;

void init_data(std::vector<ulong>& data, size_t size, ulong alpha) {
  data.resize(size);
  
  data[0] = INITIAL_FACTOR * alpha;
  for (size_t i = 1; i < size; ++i) {
    data[i] = FACTOR_B * data[i-1] + FACTOR_A;
  }
}

void print_result(std::ostream& os, std::vector<ulong>& data) {
  for (size_t i = 0; i < data.size(); i += STEP_SIZE) {
    os << data[i] << std::endl;
  }
}

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

template <typename TArray>
void heap_sort(TArray& data, int size) {
  order_like_heap(data, size);
  
  for (; size > 0; --size) {
    std::swap(data[0], data[size - 1]);
    if (size - 2 > 0) {
      order_sub_tree(data, 0, size - 2);
    }
  }
}

template <typename TArray>
void order_like_heap(TArray& data, size_t size) {
  //start with last parent node till root (data[0])
  for (int i = (size - 2) / 2; i >= 0; --i)  {
    order_sub_tree(data, i, size - 1);
  }
}

template <typename TArray>
void order_sub_tree(TArray& data, size_t start, size_t end) {
  size_t root = start;
  size_t max_item_index = root;
  while (root * 2 + 1 <= end) {
    // cmp root with first child
    if (data[root] < data[root * 2 + 1]) {
      max_item_index = root * 2 + 1;   
    }
    // cmp max_item with second child
    if (root * 2 + 2 <= end and data[max_item_index] < data[root * 2 + 2]) {
      max_item_index = root * 2 + 2;
    }
    if (max_item_index != root) {
      std::swap(data[max_item_index], data[root]);
      root = max_item_index;
    } else {
      break; // exit if no rotation made
    }
    // continue to make a subtree of max_item_index look like heap
  }
}


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

int main() {
  std::vector<ulong> data;

  init_data(data, DATA_SIZE, ALPHA);

 // qsort(data.begin(), data.end());
  heap_sort(data, data.size());
 // insertion_sort(data.begin(), data.end());  // To long to wait for. O(n^2) complexity

  std::ofstream os("outfile.txt");
  print_result(os, data);

  return 0;
}
