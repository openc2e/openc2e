#pragma once

#include <iterator>

class range {
public:
  range(int start, int end) : start_(start), end_(end) {}
  range(int end) : end_(end) {}

  class iterator
  {
    public:
      iterator(int i_) : i(i_) {}
      iterator operator++() { return iterator(i++); }
      bool operator!=(iterator other) { return i != other.i; }
      int operator*() { return i; }
      
      using difference_type = int;
      using value_type = int;
      using pointer = int;
      using reference = int;
      using iterator_category = std::forward_iterator_tag;
    private:
      int i;
  };
  
  iterator begin() const { return iterator(start_); }
  iterator end() const { return iterator(end_); }

private:
  const int start_ = 0;
  const int end_;
};