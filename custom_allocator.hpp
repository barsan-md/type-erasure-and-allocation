#pragma once

#include <cstddef>
#include <iostream>

template <class T>
struct custom_allocator {
    using value_type = T;
    custom_allocator() {
    }
    template <class U>
    custom_allocator(custom_allocator<U> const&) {
    }
    value_type* allocate(std::size_t n) {
        value_type* result = static_cast<value_type*>(::operator new(n * sizeof(value_type)));
        std::cout << "Allocator: " << this << ", allocating:   " << result << std::endl;
        return result;
    }
    void deallocate(value_type* p, std::size_t) {
        std::cout << "Allocator: " << this << ", deallocating: " << p << std::endl;
        ::operator delete(p);
    }
};
