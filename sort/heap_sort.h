#pragma once

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
