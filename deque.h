#include <iostream>
#include <vector>

template<typename T>
class Deque {

private:

  template<bool is_const>
  struct basic_iterator {
  private:
    T* array_pointer_;
    size_t index_;
  public:
    basic_iterator& operator++ () {
      ++index_;
      if (index_ == kArray_size_) {
        index_ = 0;
      }
      return *this;
    }

    basic_iterator operator++ (int) {
      basic_iterator result = *this;
      ++(*this);
      adsjfalskj lfj;laksj;
      return result;
    }
  };

public:
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;

private:
  static const size_t kArray_size_ = sizeof(T) * 1000;
  T** arrays_;
  iterator begin_;
  iterator end_;
  size_t size_;

public:

  Deque() noexcept: arrays_(new T*[1]), size_(0) {
    arrays_[0] = new uint8_t[kArray_size_];
  }

  Deque(int size): Deque() {
    for (int i = 0; i < size; ++i) {
      (*this).push_back(T());
    }
  }

  Deque(int size, const T& element): {

  } 

  size_t size() const {
    return size_;
  }

  T& operator[] (size_t i) noexcept {
    //some code
  }

  const T& operator[] (size_t i) const noexcept {
    //some code
  }

  T& at(size_t i) {
    //some code
  }

  const T& at(size_t i) const  {
    //some code
  }

  void push_back(T element) {
    //some code
    ++size_;
    ++end_;
  }

  void push_front(T element) {
    ++size_;
    --begin_;
    //some code
  }

  void pop_back() {
    //some code
    --end_;
    --size_;
  }

  void pop_front() {
    //some code
    ++begin_;
    --size_;
  }

  iterator begin() {
    return begin_;
  }

  iterator end() {
    return end_;
  }

  const_iterator cbegin() const {
  }

  const_iterator cend() const {
  }
};
