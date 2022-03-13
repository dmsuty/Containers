#include <iostream>

template<typename T>
class Deque {

private:
  template<bool is_const>
  struct basic_iterator {
  private:
    using ValueType = std::conditional<is_const, const T, T>;
    ValueType* array_pointer_;
    size_t index_;

    basic_iterator(ValueType* array_pointer, size_t index):
      array_pointer_(array_pointer), index_(index) {}

  public:
    basic_iterator& operator++ () {
      ++index_;
      if (index_ == kArray_size_) {
        index_ = 0;
        ++array_pointer_;
      }
      return *this;
    }

    basic_iterator operator++ (int) {
      basic_iterator result = *this;
      ++(*this);
      return result;
    }

    basic_iterator& operator-- () {
      if (index_ == 0) {
        index_ = kArray_size_ - 1;
        --array_pointer_;
      } else {
        --index_;
      }
      return *this;
    }

    basic_iterator operator-- (int) {
      basic_iterator result = *this;
      --(*this);
      return result;
    }

    int operator- (const basic_iterator& other) {
      int backets_between = array_pointer_ - other.array_pointer_;
      return backets_between * kArray_size_ + index_ - other.index_;
    }

    bool operator< (const basic_iterator& other) const {
      return *this - other < 0;
    }

    bool operator== (const basic_iterator& other) const {
      return *this - other == 0;
    }

    bool operator> (const basic_iterator& other) const {
      return other < *this;
    }

    bool operator!= (const basic_iterator& other) const {
      return !(*this == other);
    }

    bool operator<= (const basic_iterator& other) const {
      return !(*this > other);
    }

    bool operator>= (const basic_iterator& other) const {
      return !(*this < other);
    }

    ValueType operator* () const {
      return (*array_pointer_)[index_];
    }

    ValueType* operator-> () const {
      return *array_pointer_;
    }
  };


public:
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  static const size_t kArray_size_ = sizeof(T) * 1000;
  T** arrays_;
  iterator begin_;
  iterator end_;
  size_t backets_count_;

  void expand() {
    //TODO
  }

  void compress() {
    //TODO
  }

public:
  Deque(): arrays_(new T*[1]), begin_(iterator(arrays_[0], -1)), 
    end_(iterator(arrays_[0]), 0), backets_count_(1) {
    arrays_[0] = new uint8_t[kArray_size_];
  }

  Deque(int new_size): Deque() {
    for (int i = 0; i < new_size; ++i) {
      (*this).push_back(T());
    }
  }

  Deque(int new_size, const T& element): Deque() {
    for (int i = 0; i < new_size; ++i) {
      (*this).push_back(element);
    }
  } 

  size_t size() const {
    return end_ - begin_ - 1;
  }

  T& operator[] (size_t i) noexcept {
    //TODO
  }

  const T& operator[] (size_t i) const noexcept {
    //TODO
  }

  T& at(size_t i) {
    //TODO
  }

  const T& at(size_t i) const  {
    //TODO
  }

  void push_back(T element) {
    //TODO
    ++end_;
  }

  void push_front(T element) {
    --begin_;
    //TODO
  }

  void pop_back() {
    //TODO
    --end_;
  }

  void pop_front() {
    //TODO
    ++begin_;
  }

  iterator begin() {
    return begin_;
  }

  iterator end() {
    return end_;
  }

  iterator rbein() {
  }

  iterator rend() {
  }

  const_iterator cbegin() const {
  }

  const_iterator cend() const {
  }
};
  
