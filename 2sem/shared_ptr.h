// think later about const problem
#pragma once
#include <iostream>
#include <memory>

// think about ConstrolBlock incapsulation problem
// maybe make it a subclass of Shared, but it is not beautiful
// maybe make it subclasses of big private class, but it is stupid
// also remember about explicit
//
// also remember about explicit

// begining of declarations
template<typename T>
struct BaseBlock;

template<typename T, typename Deleter, typename Alloc>
class ControlBlock;

template<typename T, typename Alloc>
class FullControlBlock;

template<typename T>
class WeakPtr;

template<typename T>
class SharedPtr;

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc&, Args&&... args);

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args);
// ending of declarations


template<typename T> // could be without T, but I don't want to make static_cast from void* later
struct BaseBlock {
  size_t shared_count_ = 0; // read more about contructor again :(
  size_t weak_count_ = 0;

  explicit BaseBlock(size_t shared_count, size_t weak_count):
      shared_count_(shared_count), weak_count_(weak_count) {}

  template<typename Derived>
  explicit BaseBlock(const BaseBlock<Derived>& other):
      shared_count_(other.shared_count_), weak_count_(other.weak_count_) {}

  virtual ~BaseBlock() = default;

  size_t get_shared_count() { return shared_count_; }

  size_t get_weak_count() { return weak_count_; }

  void increase_shared_count() { ++shared_count_; }

  void increase_weak_count() { --weak_count_; }

  void decrease_shared_ptr() { --shared_count_; }

  void decrease_weak_ptr() { --weak_count_; }

  virtual void destroy_object() = 0;

  virtual void deallocate_block() = 0;

  virtual T* get_ptr() = 0;
};

template<typename T, typename Deleter=std::default_delete<T>, typename Alloc=std::allocator<T> >
class ControlBlock: public BaseBlock<T> {
 private:
  using ControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlock>;
  using ControlBlockTraits = std::allocator_traits<ControlBlockAlloc>;

  T* ptr_;
  Deleter deleter_;
  ControlBlockAlloc allocator_;

 public:
  explicit ControlBlock(T* ptr, const Deleter& deleter, const Alloc& allocator):
      BaseBlock<T>(1, 0), ptr_(ptr), deleter_(deleter), allocator_(allocator) {}

  T* get_ptr() override { return ptr_; }

  void destroy_object() override {
    deleter_(ptr_); // actually it should be deleter_()(...), but ikd
  }

  void deallocate_block() override {
    ControlBlockTraits::deallocate(allocator_, this, 1);
  }

  static BaseBlock<T>* create_block(T* object_ptr, const Deleter& deleter, const Alloc& allocator) {
    ControlBlockAlloc alloc = allocator;
    ControlBlock<T, Deleter, Alloc>* block_ptr = ControlBlockTraits::allocate(alloc, 1);
    ControlBlockTraits::construct(alloc, block_ptr, object_ptr, deleter, alloc);
    return block_ptr;
  }
};

template<typename T, typename Alloc=std::allocator<T> >
class FullControlBlock: public BaseBlock<T> {
 private:
  using FullControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<FullControlBlock>;
  using FullControlBlockTraits = std::allocator_traits<FullControlBlockAlloc>;
  using ObjectAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
  using ObjectTraits = std::allocator_traits<ObjectAlloc>;

  T object_;
  FullControlBlockAlloc allocator_;

 public:
  template<typename... Args>
  explicit FullControlBlock(const Alloc& allocator, Args&&... args): BaseBlock<T>(1, 0), object_(std::forward<Args>(args)...), allocator_(allocator) {}

  void destroy_object() override {
    ObjectTraits::destroy(allocator_, &object_);
  }

  void deallocate_block() override {
    FullControlBlockTraits::deallocate(allocator_, this, 1);
  }

  T* get_ptr() override { return &object_; }
};

template<typename T>
class SharedPtr {
 private:
  BaseBlock<T>* block_ptr_ = nullptr;

  template<typename Derived>
  SharedPtr(WeakPtr<Derived>& weak_ptr): block_ptr_(weak_ptr.block_ptr_) {
    block_ptr_->increase_shared_count();
  }

  template<typename Derived>
  SharedPtr(BaseBlock<Derived>* ptr): block_ptr_(ptr) {
    block_ptr_->increase_weak_count();
  }

 public:
  template<typename Derived, typename Deleter=std::default_delete<T>, typename Alloc=std::allocator<Derived> >
  explicit SharedPtr(Derived* ptr, const Deleter& deleter=Deleter(), const Alloc& allocator=Alloc()): //Derived=FCB, idk why
      block_ptr_(ControlBlock<Derived, Deleter, Alloc>::create_block(ptr, deleter, allocator)) {
    block_ptr_->increase_shared_count();
  }

  template<typename Derived>
  explicit SharedPtr(SharedPtr<Derived>&& other): block_ptr_(std::move(other.block_ptr_)) {
    other.block_ptr = nullptr;
  }

  template<typename Derived>
  explicit SharedPtr(const SharedPtr<Derived>& other): block_ptr_(other.block_ptr_) {
    block_ptr_->increase_shared_count();
  }

  template<typename Derived>
  SharedPtr operator= (SharedPtr<Derived>&& other) {
    SharedPtr copy(std::move(other));
    std::swap(block_ptr_, other.block_ptr_);
    return *this;
  }

  template<typename Derived>
  SharedPtr& operator= (const SharedPtr<Derived>& other) {
    SharedPtr copy(std::move(other));
    std::swap(block_ptr_, other.block_ptr_);
    return *this;
  }

  ~SharedPtr() {
    block_ptr_->decrease_shared_ptr();
    if (block_ptr_->get_shared_count()) {
      return;
    }
    block_ptr_->destroy_object();
    if (block_ptr_->get_weak_count() == 0) {
      block_ptr_->deallocate_block();
    }
  }

  size_t use_count() const noexcept { return block_ptr_->get_shared_count(); }

  T* operator-> () noexcept { return block_ptr_->get_ptr(); }

  const T* operator-> () const noexcept { return block_ptr_->get_ptr(); }

  T& operator* () noexcept { return *block_ptr_->get_ptr(); }

  const T& operator* () const noexcept { return *block_ptr_->get_ptr(); }

  template<typename OtherType, typename Alloc, typename... Args> //WHY OTHER TYPE HAS SOLVED THE ****ING PROBLEM
  friend SharedPtr<OtherType> allocateShared(const Alloc&, Args&&...);

  template<typename Derived>
  friend class WeakPtr;
};

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& allocator, Args&&... args) { // failed making CBT a friend, so it is what it is
  using FullControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<FullControlBlock<T, Alloc> >;
  using FullControlBlockTraits = std::allocator_traits<FullControlBlockAlloc>;
  FullControlBlockAlloc alloc = allocator;
  auto ptr = FullControlBlockTraits::allocate(alloc, 1);
  FullControlBlockTraits::construct(alloc, ptr, alloc, std::forward<Args>(args)...);
  return ptr;
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {
 private:
  BaseBlock<T>* block_ptr_;
 public:
  template<typename Derived>
  explicit WeakPtr(const SharedPtr<Derived>& shared_ptr): block_ptr_(shared_ptr.block_ptr_) {
    block_ptr_->increase_weak_count();
  }

  template<typename Derived>
  explicit WeakPtr(const WeakPtr<Derived>& other): block_ptr_(other.block_ptr_) {
    block_ptr_->increase_weak_count();
  }

  template<typename Derived>
  explicit WeakPtr(WeakPtr<Derived>&& other): block_ptr_(std::move(other.block_ptr_)) {
    other.block_ptr_ = nullptr;
  }

  template<typename Derived>
  WeakPtr& operator= (const WeakPtr<Derived>& other) {
    WeakPtr copy(other);
    std::swap(block_ptr_, copy.block_ptr_);
    return *this;
  }

  template<typename Derived>
  WeakPtr& operator= (WeakPtr<Derived>&& other) {
    WeakPtr copy(std::move(other));
    std::swap(block_ptr_, copy.block_ptr_);
    return *this;
  }

  ~WeakPtr() {
    block_ptr_->decrease_weak_ptr();
    if (block_ptr_->get_weak_count()) {
      return;
    }
    if (block_ptr_->get_shared_count() == 0) {
      block_ptr_->deallocate_block();
    }
  }

  bool expired() { return !block_ptr_->get_shared_count(); }

  SharedPtr<T> lock() {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
  }

  template<typename Derived>
  friend class SharedPtr;
};
