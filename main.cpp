#include <iostream>

#include "any.hpp"
#include "custom_allocator.hpp"

int main() {
    std::cout << "Begin" << std::endl;
    {
        custom_allocator<std::string> alloc;
        any object1(std::string("Hello"), alloc);
        any object2(std::string("World"), alloc);
        any object3(12345, alloc);
    }
    std::cout << "End" << std::endl;
    return 0;
}
