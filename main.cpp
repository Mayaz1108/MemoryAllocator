#include <iostream>
#include "MemoryAllocator.h"
#include <chrono>
#include <vector>

int main() {
    try {
        // Initialize with 1KB
        MemoryAllocator allocator(1024);
        allocator.dump_heap();

        // Test 1: Multiple Allocations
        void* p1 = allocator.allocate(100);
        void* p2 = allocator.allocate(200);
        std::cout << "\nAfter allocating 100 and 200 bytes:" << std::endl;
        allocator.dump_heap();

        // Test 2: Deallocation & Coalescing
        allocator.deallocate(p1);
        std::cout << "\nAfter deallocating first block (p1):" << std::endl;
        allocator.dump_heap();

        allocator.deallocate(p2);
        std::cout << "\nAfter deallocating second block (p2) - Blocks should merge:" << std::endl;
        allocator.dump_heap();

    } catch (const std::exception& e) {
        std::cerr << "Allocation failed: " << e.what() << std::endl;
    }
    return 0;
}

