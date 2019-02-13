#pragma once

#include <iostream>
#include <memory>

struct any {

    // Stored type
    struct type_interface;

    template <typename T>
    struct type_impl;

    // Deallocator interface
    struct deallocate_interface;

    template <typename Allocator>
    struct deallocate_wrapper;

    type_interface* value;
    deallocate_interface* deallocator;

    template <typename T, typename Allocator = std::allocator<T>>
    any(T&& value, const Allocator& allocator = Allocator()) {
        this->value       = type_impl<T>::do_allocate_construct(allocator, std::forward<T>(value));
        this->deallocator = &type_impl<T>::get_deallocator(allocator);
    }

    ~any();
};

struct any::deallocate_interface {
    virtual ~deallocate_interface() {};
    virtual void deallocate(void*) {};
};

template <typename Allocator>
struct any::deallocate_wrapper : deallocate_interface {
    Allocator& allocator;
    deallocate_wrapper(Allocator& allocator) :
            allocator(allocator) {
    }
    virtual ~deallocate_wrapper() {
    }
    virtual void deallocate(void* ptr) {
        std::allocator_traits<Allocator>::deallocate(
            allocator,
            reinterpret_cast<typename Allocator::value_type*>(ptr),
            1u);
    }
};

struct any::type_interface {
    virtual ~type_interface() {};
};

template <typename T>
struct any::type_impl : type_interface {
    T value;
    type_impl(T&& value) :
            value(value) {
        std::cout << "Constructed: " << this << ", print: " << value << std::endl;
    }
    virtual ~type_impl() {
        std::cout << "Destroyed:   " << this << ", print: " << value << std::endl;
    }
    template <typename Allocator>
    static auto& get_allocator(const Allocator& allocator) {
        using actual_allocator_t = typename std::allocator_traits<Allocator>::template rebind_alloc<type_impl<T>>;
        // static variable: initialized just on the first call
        static actual_allocator_t actual_allocator(allocator);
        return actual_allocator;
    }

    template <typename Allocator>
    static deallocate_interface& get_deallocator(Allocator& allocator) {
        auto& actual_allocator = get_allocator(allocator);
        // static variable: initialized just on the first call
        static deallocate_wrapper<std::decay_t<decltype(actual_allocator)>> deallocator(actual_allocator);
        return deallocator;
    }
    // This just call first allocate then construct using std::allocator_traits and providing the suitable allocator
    template <typename Allocator, typename... Args>
    static auto do_allocate_construct(Allocator& allocator, Args&&... args) {
        auto& actual_allocator = get_allocator(allocator);
        using allocator_t      = std::decay_t<decltype(actual_allocator)>;
        auto result            = std::allocator_traits<allocator_t>::allocate(actual_allocator, 1u);
        std::allocator_traits<allocator_t>::construct(
            actual_allocator,
            static_cast<typename allocator_t::value_type*>(result),
            std::forward<Args>(args)...);
        return result;
    }
};

any::~any() {
    this->value->~type_interface();
    this->deallocator->deallocate(this->value);
}
