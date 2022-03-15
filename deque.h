#include <iostream>

template<typename T>
class Deque {

private:
  template<bool is_const>
  struct basic_iterator {
    using ValueType = std::conditional_t<is_const, const T, T>;
    ValueType* array_pointer_;
    size_t index_;

    basic_iterator(ValueType* array_pointer, size_t index):
      array_pointer_(array_pointer), index_(index) {}

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

    basic_iterator& operator+= (int step) {
      step += index_;
      index_ = 0;
      int backets_step = step / kArray_size_;
      array_pointer_ += backets_step;
      index_ = step % kArray_size_;
      return *(this);
    }

    basic_iterator operator+ (int step) {
      basic_iterator result = (*this);
      result += step;
      return result;
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
      return array_pointer_[index_];
    }

    ValueType* operator-> () const {
      return array_pointer_;
    }

    void Construct(const T& element) {
      new(array_pointer_ + index_) T(element);
    }
  };


public:
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  static const size_t kArray_size_ = 1000;
  static const size_t kByte_Array_size_ = sizeof(T) * kArray_size_;
  T** arrays_;
  iterator begin_;
  iterator end_;
  size_t backets_count_;

  void Expand() {
    //TODO 
  }

  bool ShouldCompress() {
    return size() * 9 < backets_count_ * kArray_size_;
  }

  void Compress() {
    //TODO
  }

  iterator ToIter(size_t i) {
    return begin_ + i;
  }

  bool FreePlaceBack() {
    return (end_.array_pointer_ == arrays_ + backets_count_);
  }

  bool FreePlaceFront() {
    return (begin_.array_pointer_ == arrays_[0] && begin_.index_ == 0);

  }

public:
  Deque(): arrays_(new T*[1]), begin_(iterator(arrays_[0], -1)), 
    end_(iterator(arrays_[0], 0)), backets_count_(1) {
    arrays_[0] = reinterpret_cast<T*>(new uint8_t[kByte_Array_size_]);
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
    return end_ - begin_;
  }

  bool empty() const {
    return size() == 0;
  }

  T& operator[] (size_t i) noexcept {
    return *ToIter(i);
  }

  const T& operator[] (size_t i) const noexcept {
    return *ToIter(i);
  }

  T& at(size_t i) {
    return (*this)[i];
  }

  const T& at(size_t i) const  {
    return (*this)[i];
  }

  void push_back(const T& element) {
    if (!FreePlaceBack()) {
      Expand();
    }
    end_.Construct(element);  
    ++end_;
  }

  void push_front(const T& element) {
    if (!FreePlaceFront()) {
      Expand();
    }
    --begin_;
    begin_.Construct(element);
  }

  void pop_back() {
    --end_;
    if (ShouldCompress()) {
      Compress();
    }
  }

  void pop_front() {
    ++begin_;
    if (ShouldCompress()) {
      Compress();
    }
  }

  iterator begin() {
    return begin_;
  }

  iterator end() {
    return end_;
  }

  iterator rbegin() {
    return std::reverse_iterator<iterator>(end_ - 1);
  }

  iterator rend() {
    //TODO
  }

  const_iterator cbegin() const {
    return const_iterator(begin_);
  }

  const_iterator cend() const {
    return const_iterator(end_);
  }

  const_reverse_iterator crbegin() const {
    //TODO
  }

  const_reverse_iterator crend() const {
    //TODO
  }
};
  
