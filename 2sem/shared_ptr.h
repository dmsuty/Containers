#pragma once
#include <iostream>
#include <optional>
#include <memory>

// begining of declarations
struct BaseBlock;

template<typename T, typename Alloc, typename Deleter>
class ControlBlock;

template<typename T>
class WeakPtr;

template<typename T>
class SharedPtr;

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc, Args&&... args);

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&& ... args);
// ending of declarations


struct BaseBlock {
  virtual size_t get_shared_count();

  virtual size_t get_weak_count();

  virtual void increase_shared_count();

  virtual void increase_weak_count();
};

template<typename T, typename Alloc=std::allocator<T>, typename Deleter=std::default_delete<T> >
class ControlBlock: public BaseBlock {
 private:
  using ControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlock>;
  using ControlBlockTraits = std::allocator_traits<Alloc>;
  size_t shared_count;
  size_t weak_count;
  T* ptr;
  std::optional<T> object;

 public:
  ControlBlock(T* ptr): shared_count(1), weak_count(0) {} // default optional without value

  size_t get_shared_count() { return shared_count; }

  size_t get_weak_count() { return weak_count; }
};

template<typename T>
class SharedPtr {
 private:
  BaseBlock* block_ptr_;

 public:
  SharedPtr(T* ptr) {}

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

  SharedPtr& operator= (SharedPtr&& other) {} // mb, it is not necesarry

  SharedPtr& operator= (const SharedPtr& other) {} // mb, it is not necesarry

  template<typename Derived>
  SharedPtr operator= (SharedPtr<Derived>&& other) {}

  template<typename Derived>
  SharedPtr& operator= (const SharedPtr<Derived>& other) {}

  ~SharedPtr(); // TODO depends on was makeShared or not

  size_t use_count() { return block_ptr_->get_shared_count(); }

  template<typename Alloc, typename... Args>
  friend SharedPtr<T> allocateShared(Alloc, Args&&... args);
};

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc& allocator, Args&&... args) {
  //TODO
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {

};
