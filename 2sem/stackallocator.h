#include <iostream>
#include <cstddef>
#include <iterator>

template<size_t N>
class alignas(std::max_align_t) StackStorage {
 private:
  uint8_t pool_[N];
  size_t free_index_ = 0;

 public:
  StackStorage() = default;

  uint8_t* GetMemory(size_t bytes, size_t align) {
    //TODO free place is not endless
    if (free_index_ % align != 0) {
      free_index_ += align - (free_index_ % align);
    }
    free_index_ += bytes;
    return pool_ + (free_index_ - bytes);
  }
};


template<typename T, size_t N>
class StackAllocator {
 private:
  StackStorage<N>* storage_pointer_;

 public:
  using value_type = T;

  StackAllocator() = delete;

  StackAllocator(const StackAllocator& other) = default;

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& other):
      storage_pointer_(other.storage_pointer_) {}

  //mb should use const_cast<...>, but idk what's the point
  StackAllocator(StackStorage<N>& storage): storage_pointer_(&storage) {}

  StackAllocator& operator= (const StackAllocator& other) = default;

  ~StackAllocator() = default;

  T* allocate(size_t n) {
    return reinterpret_cast<T*>(storage_pointer_->GetMemory(n * sizeof(T), alignof(T)));
  }

  void deallocate(T* ptr, size_t n) {}

  template<typename U, size_t S>
  bool operator== (StackAllocator<U, S>& other) {
    return storage_pointer_ == other.storage_pointer_;
  }

  template<typename U, size_t S>
  bool operator!= (StackAllocator<U, S>& other) {
    return storage_pointer_ != other.storage_pointer_;
  }

  //in theory it's not necesarry(alloc_traits), why not???
  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };
};


template<typename T, typename Allocator=std::allocator<T> >
class List {
 private:
  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;
  };

  struct Node : BaseNode {
    T value;
  };

  template<typename ValueType> //mb <bool is_const>
  struct basic_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    BaseNode* node_ptr;

    basic_iterator& operator-- () {
      node_ptr = node_ptr->prev;
      return *this;
    }

    basic_iterator operator-- (int) {
      basic_iterator curr_iter(*this);
      --(*this);
      return curr_iter;
    }

    basic_iterator& operator++ () {
      node_ptr = node_ptr->prev;
      return *this;
    }

    basic_iterator operator++ (int) {
      basic_iterator curr_iter(*this);
      ++(*this);
      return curr_iter;
    }

    basic_iterator operator+ (int step) {
      basic_iterator result(*this);
      for (int i = 0; i < step; ++i) {
        ++result;
      }
      for (int i = 0; i < -step; ++i) {
        --result;
      }
      return result;
    }

  };

   //some logic
 public:
  using iterator = basic_iterator<T>;
  using const_iterator = basic_iterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List() {}

  List(size_t size) {}

  List(size_t size, const T& default_element) {}

  List(Allocator alloc) {}

  List(size_t size, Allocator alloc) {}

  List(size_t size, const T& default_element, Allocator alloc) {}

  List(const List& other) {}

  List& operator= (const List& other) {}

  ~List() {}

  Allocator get_allocator() {} const

  void push_back(const T& element) {}

  void push_front(const T& element) {}

  void pop_back(const T& element) {}

  void pop_front(const T& element) {}

  void insert(iterator iter, const T& element) {}

  void erase(iterator iter) {}

  iterator begin() {}

  iterator end() {}

  const_iterator cbegin() {} const

  const_iterator cend() {} const

  reverse_iterator rbegin() {}

  reverse_iterator rend() {}

  //crbegin and crend
};
