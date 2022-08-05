//TODO exceptions!!!
#include <iostream>
#include <cstddef>
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

  template<typename U, size_t M>
  friend class StackAllocator;

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

    BaseNode(): prev(this), next(this) {} //should read more about constructors
  };

  struct Node : BaseNode {
    T value;

    Node(const T& element): value(element) {}
  };

  template<bool is_const>
  struct basic_iterator {
    using value_type = typename std::conditional<is_const, const T, T>::type;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    using NodeRef = typename std::conditional<is_const, const Node&, Node&>::type;
    using NodePtr = typename std::conditional<is_const, const Node*, Node*>::type;
    using BaseNodeRef = typename std::conditional<is_const, const BaseNode&, BaseNode&>::type;
    using BaseNodePtr = typename std::conditional<is_const, const BaseNode*, BaseNode*>::type;

    BaseNodePtr base_node_ptr;

    basic_iterator() = default;

    basic_iterator(BaseNodeRef base_node): base_node_ptr(&base_node) {}

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
      base_node_ptr = base_node_ptr->next;
      return *this;
    }

    basic_iterator operator++ (int) {
      basic_iterator curr_iter(*this);
      ++(*this);
      return curr_iter;
    }

    basic_iterator operator+ (int step) const {
      basic_iterator result(*this);
      for (int i = 0; i < step; ++i) {
        ++result;
      }
      for (int i = 0; i < -step; ++i) {
        --result;
      }
      return result;
    }

    basic_iterator operator- (int step) const {
      return (*this) + (-step);
    }

    pointer operator->() const {
      return static_cast<NodePtr>(base_node_ptr)->value;
    }

    reference operator* () const {
      return static_cast<NodePtr>(base_node_ptr)->value;
    }

    bool operator== (const basic_iterator& other) const {
      return base_node_ptr == other.base_node_ptr;
    }

    bool operator!= (const basic_iterator& other) const {
      return !(*this == other);
    }

    operator basic_iterator<true>() const {
      return basic_iterator<true>(*base_node_ptr);
    }

   private:
    BaseNodeRef get_node() {
      return *base_node_ptr;
    }

    BaseNodePtr next() {
      return base_node_ptr->next;
    }

    BaseNodePtr prev() {
      return base_node_ptr->prev;
    }

    friend class List; //mb there is another way
  };

 public: //use iterator_traits
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>; //why typename and template
  using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

 private:
  size_t size_ = 0; //should I do this if it is default value(fedor's ans)
  BaseNode fake_node_ = BaseNode();
  NodeAlloc allocator_ = NodeAlloc();

  void connect(BaseNode* first, BaseNode* second) {
    first->next = second;
    second->prev = first;
  }

  void my_swap(List& other) {
    std::swap(size_, other.size_);
    std::swap(allocator_, other.allocator_);
    fake_nodes_swap(fake_node_, other.fake_node_);
  }

  void fake_nodes_swap(BaseNode& fake1, BaseNode& fake2) {
    std::swap(fake1, fake2);
    fake1.next->prev = &fake1;
    fake1.prev->next = &fake1;
    fake2.next->prev = &fake2;
    fake2.prev->next = &fake2;
  }

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
    for (const T& el : other) {
      push_back(el);
    }
  }

  List& operator= (const List& other) {
    if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
      allocator_ = other.get_allocator();
    } else {
      allocator_ = NodeAllocTraits::select_on_container_copy_construction(other.get_allocator());
    }
    List copy(allocator_);
    for (const T& el: other) {
      copy.push_back(el);
    }
    my_swap(copy); //idk why, but copy is broken instead of *this
    return *this;
  }

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
    ++size_;
    BaseNode* old_next_node_ptr = iter.next();
    Node* new_next_node_ptr = NodeAllocTraits::allocate(allocator_, 1);
    NodeAllocTraits::construct(allocator_, new_next_node_ptr, element);
    connect(iter.base_node_ptr, new_next_node_ptr);
    connect(new_next_node_ptr, old_next_node_ptr);
  }

  void erase(iterator iter) {
    --size_;
    connect(iter.prev(), iter.next());
    NodeAllocTraits::destroy(allocator_, static_cast<Node*>(iter.base_node_ptr)); //mb should make function for the cast
    NodeAllocTraits::deallocate(allocator_, static_cast<Node*>(iter.base_node_ptr), 1);
  }

  void push_back(const T& element) {
    insert(end() - 1, element);
  }

  void push_front(const T& element) {
    insert(end(), element);
  }

  void pop_back() {
    erase(end() - 1);
  }

  void pop_front(const T& element) {
    erase(begin());
  }

  T& operator[] (size_t i) { // doesn't work in GDB
    return *(begin() + i);
  }

  const T& operator[] (size_t i) const { // the same problem with GDB
    return *(begin() + i);
  }

  iterator begin() {
    return iterator(*fake_node_.next);
  }

  iterator end() {
    return iterator(fake_node_);
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  const_iterator cbegin() const {
    return const_iterator(*fake_node_.next);
  }

  const_iterator cend() const {
    return const_iterator(fake_node_);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crbegin() const {
    return reverse_iterator(cend());
  }

  const_reverse_iterator crend() const {
    return reverse_iterator(cbegin());
  }
};
