#include <iostream>

template<typename T>
class Deque {

private:
  template<bool is_const>
  struct basic_iterator {
    using ValueType = std::conditional_t<is_const, const T, T>;
    using Pointer = ValueType*;
    using DoublePointer = std::conditional_t<is_const, const Pointer*, T**>;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename std::conditional<is_const, const T, T>::type;

    DoublePointer backet_pointer_;
    Pointer array_pointer_;
    size_t index_;

    explicit basic_iterator() = default;

    explicit basic_iterator(DoublePointer backet_pointer, Pointer array_pointer, int index):
      backet_pointer_(backet_pointer), array_pointer_(array_pointer), index_(index) {}

    explicit basic_iterator(DoublePointer backet_pointer):
      backet_pointer_(backet_pointer), array_pointer_(*backet_pointer_), index_(0) {}

    basic_iterator& operator++ ()  noexcept {
      ++index_;
      if (index_ == kArray_size_) {
        backet_pointer_++;
        array_pointer_ = *backet_pointer_;
        index_ = 0;
      }
      return *this;
    }

    basic_iterator operator++ (int) noexcept {
      basic_iterator result = *this;
      ++(*this);
      return result;
    }

    basic_iterator& operator-- () noexcept {
      if (index_ == 0) {
        backet_pointer_--;
        array_pointer_ = *backet_pointer_;
        index_ = kArray_size_ - 1;
      } else {
        --index_;
      }
      return *this;
    }

    basic_iterator operator-- (int) noexcept {
      basic_iterator result = *this;
      --(*this);
      return result;
    }

    long int operator- (const basic_iterator& other) const noexcept {
      long int backets_between = backet_pointer_ - other.backet_pointer_;
      return backets_between * kArray_size_ + index_ - other.index_;
    }

    basic_iterator& operator+= (long long step) noexcept {
      step += index_;
      int backets_step = step / kArray_size_;
      backet_pointer_ += backets_step;
      array_pointer_ = *backet_pointer_;
      index_ = step % kArray_size_;
      return *(this);
    }

    basic_iterator operator+ (int step) const noexcept {
      basic_iterator result = *this;
      result += step;
      return result;
    }

    basic_iterator& operator-= (int step) noexcept {
      step += kArray_size_ - index_ - 1;
      int backets_step = step / kArray_size_;
      backet_pointer_ -= backets_step;
      array_pointer_ = *backet_pointer_;
      index_ = kArray_size_ - 1 - step % kArray_size_;
      return *(this);
    }

    basic_iterator operator- (int step) const noexcept {
      basic_iterator result = *this;
      result -= step;
      return result;
    }

    bool operator< (const basic_iterator& other) const noexcept {
      return *this - other < 0;
    }

    bool operator== (const basic_iterator& other) const noexcept {
      return *this - other == 0;
    }

    bool operator> (const basic_iterator& other) const noexcept {
      return other < *this;
    }

    bool operator!= (const basic_iterator& other) const noexcept {
      return !(*this == other);
    }

    bool operator<= (const basic_iterator& other) const noexcept {
      return !(*this > other);
    }

    bool operator>= (const basic_iterator& other) const noexcept {
      return !(*this < other);
    }

    ValueType& operator* () const noexcept {
      return array_pointer_[index_];
    }

    Pointer operator-> () const noexcept {
      return array_pointer_ + index_;
    }

    void construct(const T& element) {
      try {
        new(array_pointer_ + index_) T(element);
      } catch (...) {
        throw;
      }
    }

    void deconstruct() {
      T& el = *(*this);
      el.~T();
    }

    operator basic_iterator<true>() const {
      return basic_iterator<true>(backet_pointer_, array_pointer_, index_);
    }
  };


public:
  using value_type = T;
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  static const size_t kArray_size_ = 10'000;
  static const size_t kByte_Array_size_ = sizeof(T) * kArray_size_;
  size_t backets_count_;
  T** backets_;
  iterator begin_;
  iterator end_;

  void expand() {
    size_t done_news = 0;
    T** new_arrays;
    try {
      new_arrays = new T*[backets_count_ * 3];
      for (size_t i = 0; i < backets_count_ * 3; ++i) {
        if (i < backets_count_ || (i >= backets_count_ * 2)) {
          new_arrays[i] = reinterpret_cast<T*>(new uint8_t[kByte_Array_size_]);
          ++done_news;
        } else {
          new_arrays[i] = backets_[i - backets_count_];
        }
      }
      update_iterator_after_expand(new_arrays, begin_);
      update_iterator_after_expand(new_arrays, end_);
      delete[] backets_;
      backets_ = new_arrays;
      backets_count_ *= 3;
    } catch (...) {
      for (size_t i = 0; i < backets_count_ * 3; ++i) {
        if (i < done_news && (i < backets_count_ || i >= backets_count_ * 2)) {
          delete[] reinterpret_cast<uint8_t*>(new_arrays[i]);
        }
      }
      delete new_arrays;
      throw;
    }
  }

  void update_iterator_after_expand(T** new_arrays, iterator& iter) {
    int backets_number = iter.backet_pointer_ - backets_;
    iter.backet_pointer_ = new_arrays + (backets_number + backets_count_);
    iter.array_pointer_ = *(iter.backet_pointer_);
  }

  iterator to_iter(int i) const noexcept {
    return begin_ + i;
  }

  bool free_place_back() noexcept {
    return !(end_.backet_pointer_ == backets_ + backets_count_ - 1 && end_.index_ == kArray_size_ - 1);
  }

  bool free_place_front() noexcept {
    return !(begin_.backet_pointer_ == backets_ && begin_.index_ == 0);
  }

  void swap(Deque& other) noexcept {
    std::swap(backets_, other.backets_);
    std::swap(backets_count_, other.backets_count_);
    std::swap(begin_, other.begin_);
    std::swap(end_, other.end_);
  }

  void clear() {
    while (size()) {
      pop_back();
    }
  }

public:
  Deque(): backets_count_(1), backets_(new T*[1]) {
    try {
      backets_[0] = reinterpret_cast<T*>(new uint8_t[kByte_Array_size_]);
      begin_ = iterator(backets_) + kArray_size_ / 2;
      end_ = begin_;
    } catch (...) {
      delete[] backets_;
      throw;
    }
  }

  Deque(int new_size): Deque() {
    try {
      for (int i = 0; i < new_size; ++i) {
        push_back(T());
      }
    } catch (...) {
      throw;
    }
  }

  Deque(int new_size, const T& element): Deque() {
    try {
      for (int i = 0; i < new_size; ++i) {
        (*this).push_back(element);
      }
    } catch (...) {
      throw;
    }
  }

  Deque(const Deque& other):
      backets_count_(other.backets_count_), backets_(new T*[backets_count_]) {
    try {
      for (size_t i = 0; i < backets_count_; ++i) {
        backets_[i] = reinterpret_cast<T*>(new uint8_t[kByte_Array_size_]);
      }
      begin_ = iterator(backets_) + (other.begin_ - iterator(other.backets_));
      end_ = begin_ + other.size();
      iterator curr_iter = begin_;
      iterator other_iter = other.begin_;
      while (curr_iter != end_) {
        curr_iter.construct(*other_iter);
        ++curr_iter;
        ++other_iter;
      }
    } catch (...) {
      if (backets_count_ == other.backets_count_) {
        clear();
      }
      for (size_t curr_backet = 0; curr_backet < backets_count_; ++curr_backet) {
        delete[] reinterpret_cast<uint8_t*>(backets_[curr_backet]);
      }
      delete[] backets_;
      throw;
    }
  }

  ~Deque() {
    clear();
    for (size_t i = 0; i < backets_count_; ++i) {
      delete[] reinterpret_cast<uint8_t*>(backets_[i]);
    }
    delete[] backets_;
  }

  Deque& operator=(const Deque &other) {
    Deque copy(other);
    swap(copy);
    return *this;
  }

  size_t size() const noexcept {
    return end_ - begin_;
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  T& operator[] (size_t i) noexcept {
    return *(to_iter(i));
  }

  const T& operator[] (size_t i) const noexcept {
    return *(to_iter(i));
  }

  T& at(size_t i) {
    if (i >= size()) {
      throw std::out_of_range("Out of range ;(");
    }
    return (*this)[i];
  }

  const T& at(size_t i) const {
    if (i >= size()) {
      throw std::out_of_range("Out of range ;(");
    }
    return (*this)[i];
  }

  void push_back(const T& element) {
    if (!free_place_back()) {
      expand();
    }
    end_.construct(element);
    ++end_;
  }

  void push_front(const T& element) {
    try {
      if (!free_place_front()) {
        expand();
      }
      --begin_;
      begin_.construct(element);
    } catch (...) {
      throw;
    }
  }

  void pop_back() {
    if (size() == 0) {
      throw;
    }
    end_.deconstruct();
    --end_;
  }

  void pop_front() {
    if (size() == 0) {
      throw;
    }
    begin_.deconstruct();
    ++begin_;
  }

  void erase(iterator iter) {
    if (size() == 0) {
      throw;
    }
    Deque start_copy = *this;
    for (iterator curr_iter = iter; curr_iter + 1 != end_; curr_iter++) {
      std::swap(*(curr_iter), *(curr_iter + 1));
    }
    pop_back();
  }

  void insert(iterator iter, const T& element) {
    Deque start_copy = *this;
    try {
      int shift = iter - begin_;
      push_back(element);
      for (iterator curr_iter = end_ - 1; curr_iter - begin_ != shift; curr_iter--) {
        std::swap(*(curr_iter), *(curr_iter - 1));
      }
    } catch (...) {
      swap(start_copy);
      throw;
    }
  }

  iterator begin() noexcept {
    return begin_;
  }

  iterator end() noexcept {
    return end_;
  }

  const_iterator begin() const noexcept {
    return cbegin();
  }

  const_iterator end() const noexcept {
    return cend();
  }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator(end_);
  }

  reverse_iterator rend() noexcept {
    return reverse_iterator(begin_);
  }

  const_iterator cbegin() const  noexcept {
    return const_iterator(begin_);
  }

  const_iterator cend() const  noexcept {
    return const_iterator(end_);
  }

  const_reverse_iterator crbegin() const noexcept {
    return std::make_reverse_iterator(cbegin());
  }

  const_reverse_iterator crend() const noexcept {
    return std::make_reverse_iterator(cend());
  }
};

/*
132-134 строки ничего не делают. если ты просто ловишь и перебрасываешь исключение, с тем же успехом его можно просто не ловить. ну не очень хорошо что это метод итератора, причём публичный
273 всегда true
277 не учитывает сколько реально бакетов успели аллоцироваться до исключения
самый простой способ безопасно реализовать эту функцию - через пушбеки
341 и 342 строки нужно поменять местами и вообще убрать try catch
350 - так делать нельзя, просто throw; без активного исключения скрашит программу
в erase явно не хватает кода, потому что start_copy создаётся, но не используется, но идейно тоже самое, что и insert :)
вообще код довольно приятный, чистый.
*/
