// think later about const problem
#pragma once
#include <iostream>
#include <memory>

// think about ConstrolBlock incapsulation problem
// maybe make it a subclass of Shared, but it is not beautiful

// begining of declarations
template<typename T>
struct BaseBlock;

template<typename T, typename Deleter, typename Alloc>
class ControlBlock;

template<typename T, typename Alloc>
class ControlBlockWithObject;

template<typename T>
class WeakPtr;

template<typename T>
class SharedPtr;

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc, Args&&... args);

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args);
// ending of declarations


template<typename T> // could be without T, but I don't want to make static_cast from void* later
struct BaseBlock {
  size_t shared_count_; // read more about contructor again :(
  size_t weak_count_;

  BaseBlock(size_t shared_count, size_t weak_count):
      shared_count_(shared_count), weak_count_(weak_count) {}

  template<typename Derived>
  BaseBlock(const BaseBlock<Derived>& other):
      shared_count_(other.shared_count_), weak_count_(other.weak_count_) {}

  size_t get_shared_count() { return shared_count_; }

  size_t get_weak_count() { return weak_count_; }

  void increase_shared_count() { ++shared_count_; }

  void increase_weak_count() { --weak_count_; }

  void decrease_shared_ptr() { --shared_count_; }

  void decrease_weak_ptr() { --weak_count_; }

  virtual void destroy_object() = 0;

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
  ControlBlock(T* ptr, Deleter& deleter, Alloc& allocator):
      BaseBlock<T>(1, 0), ptr_(ptr), deleter_(deleter),
      allocator_(ControlBlockTraits::select_on_container_copy_construction(allocator)) {}

  void* get_ptr() override { return ptr_; }

  void destroy_object() override {
    deleter_(ptr_);
  }

  static BaseBlock<T>* create_block(T* object_ptr, Deleter deleter, Alloc allocator) {
    ControlBlock* block_ptr = ControlBlockTraits::allocate(allocator, 1);
    ControlBlockTraits::construct(allocator, block_ptr, object_ptr, deleter, allocator);
    return block_ptr;
  }
};

template<typename T, typename Alloc=std::allocator<T> >
class ControlBlockWithObject: public BaseBlock<T> {
 private:
  using ControlBlockWithObjectAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockWithObject>;
  using ControlBlockWithObjectTraits = std::allocator_traits<ControlBlockWithObjectAlloc>;
  using ObjectAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
  using ObjectTraits = std::allocator_traits<ObjectAlloc>;

  T object_;
  ControlBlockWithObjectAlloc allocator_;

 public:
  template<typename... Args>
  ControlBlockWithObject(Alloc& allocator, Args&&... args): BaseBlock<T>(1, 0), object_(std::forward<Args>(args)...),
      allocator_(ControlBlockWithObjectTraits::select_on_container_copy_construction(allocator)) {}

  void destroy_object() override {
    ObjectTraits::destroy(allocator_, &object_);
  }

  void* get_ptr() override { return &object_; }

  friend allocateShared() // for usings, make it later
};

template<typename T>
class SharedPtr {
 private:
  BaseBlock<T>* block_ptr_ = nullptr;

  T& get_ref() { return *block_ptr_->get_ptr(); }

  void deallocate_block() {
    
  }

 public:
  SharedPtr() = default; // update other methods later

  SharedPtr(BaseBlock<T>* ptr): block_ptr_(ptr) {}

  template<typename Derived, typename Deleter=std::default_delete<T>, typename Alloc=std::allocator<Derived> >
  SharedPtr(Derived* ptr, Deleter& deleter=std::default_delete<Derived>(), Alloc& allocator=std::allocator<Derived>()):
      block_ptr_(ControlBlock<Derived, Deleter, Alloc>::create_block(ptr, deleter, allocator)) {}

  template<typename Derived>
  SharedPtr(SharedPtr<Derived>&& other): block_ptr_(std::move(other.block_ptr_)) {
    other.block_ptr = nullptr;
  }

  template<typename Derived>
  SharedPtr(const SharedPtr<Derived>& other): block_ptr_(other.block_ptr_) {
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
    if (block_ptr_->get_weak_count()) {
      block_ptr_->destroy_object();
    } else {
      deallocate_block();
    }
  }

  size_t use_count() { return block_ptr_->get_shared_count(); }

  template<typename Alloc, typename... Args>
  friend SharedPtr<T> allocateShared(Alloc&, Args&&...);
};

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc& allocator, Args&&... args) { // failed making CBT a friend, so it is what it is
  using ControlBlockWithObjectAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockWithObject>;
  using ControlBlockWithObjectTraits = std::allocator_traits<ControlBlockWithObjectAlloc>;
  ControlBlockWithObject<T, Alloc>* ptr = ControlBlockWithObjectTraits::allocate(allocator, 1); // mb could use BaseBlock*
  ControlBlockWithObjectTraits::construct(allocator, ptr, allocator, std::forward<Args>(args)...);
  return SharedPtr<T>(ptr);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {

};
