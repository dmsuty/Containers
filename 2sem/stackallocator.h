//TODO exceptions!!!
#include <iostream>
#include <cstddef>
#include <iterator>
#include <memory>

template<size_t N>
class alignas(std::max_align_t) StackStorage {
 private:
  uint8_t pool_[N];
  size_t free_index_ = 0;

 public:
  StackStorage() = default;

  uint8_t* GetMemory(size_t bytes, size_t align) {
    //TODO free place is not endless(for exceptions)
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

  //alloc_traits isn't good enough, so I need to write it
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

    BaseNode(): prev(this), next(this) {}
  };

  struct Node : BaseNode {
    T value;
  };

  template<typename value_type> //mb <bool is_const>
  struct basic_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    BaseNode* base_node_ptr;

    basic_iterator& operator-- () {
      base_node_ptr = base_node_ptr->prev;
      return *this;
    }

    basic_iterator operator-- (int) {
      basic_iterator curr_iter(*this);
      --(*this);
      return curr_iter;
    }

    basic_iterator& operator++ () {
      base_node_ptr = base_node_ptr->prev;
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

    basic_iterator operator- (int step) {
      return (*this) + (-step);
    }

    value_type& operator* () {
      Node* node_ptr = static_cast<Node*>(base_node_ptr);
      return node_ptr->value;
    }

  };

 public:
  using iterator = basic_iterator<T>;
  using const_iterator = basic_iterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>; //why typename and template
  using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

 private:
  size_t size_ = 0;  //should I do this if it is default value(fedor's ans)
  BaseNode fake_node_ = BaseNode();
  NodeAlloc allocator_ = NodeAlloc();

 public:
  List() = default;

  List(size_t size, const T& default_element) {
    for (size_t i = 0; i < size; ++i) {
      push_back(default_element);
    }
  }

  List(size_t size): List(size, T()) {}

  List(Allocator alloc): allocator_(NodeAllocTraits::select_on_container_copy_construction(alloc)) {}

  List(size_t size, const T& default_element, Allocator alloc): List(alloc) {
    for (size_t i = 0; i < size; ++i) {
      push_back(default_element);
    }
  }

  List(size_t size, Allocator alloc): List(size, T(), alloc) {}

  List(const List& other): List(other.get_allocator()) {
    for (T& el : other) { //may use auto, but I liked T more
      push_back(el);
    }
  }

  //List& operator= (const List& other): {} //what to do with allocator and don't forget to clean

  ~List() {
    while (size_) {
      pop_back();
    }
  }

  size_t size() const {
    return size_;
  }

  Allocator get_allocator() const {
    return allocator_;
  }

  void insert(iterator iter, const T& element) {
    iter->next = NodeAllocTraits::allocate(allocator_);
    static_cast<Node*>(iter->next)->value = element;
  }

  void erase(iterator iter) {}

  void push_back(const T& element) {
    insert(end() - 1, element);
  }

  void push_front(const T& element) {
    insert(begin(), element);
  }

  void pop_back(const T& element) {
    erase(end() - 1);
  }

  void pop_front(const T& element) {
    erase(begin());
  }

  iterator begin() {}

  iterator end() {}

  const_iterator cbegin() {} const

  const_iterator cend() {} const

  reverse_iterator rbegin() {}

  reverse_iterator rend() {}

  //crbegin and crend
};
