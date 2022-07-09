#include <iostream>

template<typename T, size_t N>
class StackAllocator {
private:
    StackStorage* storage_pointer;
public:
    StackAllocator() {}

    StackAllocator(const StackAllocator& other): storage_pointer =  {}

    ~StackAllocator() {}

    StackAllocator& operator= (const StackAllocator& other) {

    }

    void allocate() {

    }

    void deallocate() {

    }
};

template<size_t N>
class StackStorage {
private:
    uint8_t array;
public:
    StackStorage(): array(N) {}
};

template<typename T, typename Allocator=std::allocator<T> >
class List {
public:
    template<typename ValueType>
    struct basic_iterator {

    };

    using iterator = basic_iterator<T>;
    using const_iterator = basic_iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
    //some logic
public:
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
