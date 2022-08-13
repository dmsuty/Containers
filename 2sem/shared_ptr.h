// think later about const problem
#pragma once
#include <iostream>
#include <optional>
#include <memory>

// begining of declarations
struct BaseBlock;

template<typename T, typename Deleter, typename Alloc>
class ControlBlock;

template<typename T>
class WeakPtr;

template<typename T>
class SharedPtr;

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc, Args&&... args);

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args);
// ending of declarations


struct BaseBlock {
  virtual size_t get_shared_count() = 0;

  virtual size_t get_weak_count() = 0;

  virtual void increase_shared_count() = 0;

  virtual void increase_weak_count() = 0;

  virtual void decrease_shared_ptr() = 0;

  virtual void decrease_weak_ptr() = 0;

  virtual void* get_ptr() = 0;
};

template<typename T, typename Deleter=std::default_delete<T>, typename Alloc=std::allocator<T> >
class ControlBlock: public BaseBlock {
 private:
  using ControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlock>;
  using ControlBlockTraits = std::allocator_traits<ControlBlockAlloc>;
  size_t shared_count_ = 0; // read more about contructor again :(
  size_t weak_count_ = 0;
  T* ptr_ = nullptr;
  std::optional<T> value_;
  Deleter deleter_ = Deleter();
  ControlBlockAlloc allocator_ = Alloc();

 public:
  ControlBlock(T* ptr, Deleter& deleter, Alloc& allocator):
      shared_count_(1), ptr_(ptr), deleter_(deleter),
      allocator_(ControlBlockTraits::select_on_container_copy_construction(allocator)) {} // default optional without value

  template<typename... Args>
  ControlBlock(Alloc& allocator, Args&&... args): shared_count_(1), value_(std::forward<Args>(args)...),
      ptr_(&value_.value()), allocator_(ControlBlockTraits::select_on_container_copy_construction(allocator)) {
    std::cout << "aaaa" << '\n';
    }

  size_t get_shared_count() override { return shared_count_; }

  size_t get_weak_count() override { return weak_count_; }

  void increase_shared_count() override { ++shared_count_; }

  void increase_weak_count() override { --weak_count_; }

  void decrease_shared_ptr() override { --shared_count_; }

  void decrease_weak_ptr() override { --weak_count_; }

  void* get_ptr() override { return ptr_; }

  static BaseBlock* create_block(T* value_ptr, Deleter deleter, Alloc allocator) {
    ControlBlock* block_ptr = ControlBlockTraits::allocate(allocator, 1);
    ControlBlockTraits::construct(allocator, block_ptr, value_ptr, deleter, allocator);
    return block_ptr;
  }

  template<typename... Args>
  friend SharedPtr<T> allocateShared(Alloc&, Args&&...);
};

template<typename T>
class SharedPtr {
 private:
  BaseBlock* block_ptr_ = nullptr;

  T* get_ptr() { return static_cast<T*>(block_ptr_->get_ptr()); }

  T& get_ref() { return *get_ptr(); }

 public:
  SharedPtr() = default; // update other methods later

  SharedPtr(BaseBlock* ptr): block_ptr_(ptr) {}

  template<typename Deleter=std::default_delete<T>, typename Alloc=std::allocator<T> >
  SharedPtr(T* ptr, Deleter& deleter=std::default_delete<T>(), Alloc& allocator=std::allocator<T>()):
      block_ptr_(ControlBlock<T, Deleter, Alloc>::create_block(ptr, deleter, allocator)) {}

  SharedPtr(SharedPtr&& other): block_ptr_(std::move(other.block_ptr_)) {
    other.block_ptr_ = nullptr;
  } // mb, it is not necesarry

  SharedPtr(const SharedPtr& other): block_ptr_(other.block_ptr_) {
    block_ptr_->increase_shared_count();
  } // mb, it is not necesarry

  template<typename Derived>
  SharedPtr(SharedPtr<Derived>&& other): block_ptr_(std::move(other.block_ptr_)) {
    other.block_ptr = nullptr;
  }

  template<typename Derived>
  SharedPtr(const SharedPtr<Derived>& other): block_ptr_(other.block_ptr_) {
    block_ptr_->increase_shared_count();
  }

  SharedPtr& operator= (SharedPtr&& other) {
    SharedPtr copy(std::move(other));
    std::swap(block_ptr_, other.block_ptr_);
    return *this;
  } // mb, it is not necesarry

  SharedPtr& operator= (const SharedPtr& other) {
    SharedPtr copy(std::move(other));
    std::swap(block_ptr_, other.block_ptr_);
    return *this;
  } // mb, it is not necesarry

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
    if (block_ptr_->get_shared_count()) return;
    if (get_ref().has_value()) {
      if (block_ptr_->get_weak_count()) {
        // TODO
      } else {
        // TODO
      }
    } else {
      if (block_ptr_->get_weak_count()) {
        // TODO
      } else {
        // TODO
      }
    }
  }
// shared_count == 0 -> deleter
// shared_count == weak_count == 0 -> deallocate
// depends on how it was created

  size_t use_count() { return block_ptr_->get_shared_count(); }

  template<typename Alloc, typename... Args>
  friend SharedPtr<T> allocateShared(Alloc&, Args&&...);
};

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc& allocator, Args&&... args) { // failed making CBT a friend, so it is what it is
  using ControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlock>;
  using ControlBlockTraits = std::allocator_traits<ControlBlockAlloc>;
  ControlBlock<T>* ptr = ControlBlockTraits::allocate(allocator, 1); // mb could use BaseBlock*
  ControlBlockTraits::construct(allocator, ptr, std::forward<Args>(args)...);
  return SharedPtr<T>(ptr);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {

};
