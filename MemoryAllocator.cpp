#include "MemoryAllocator.h"

#include <cstdlib>    
#include <cstdint>
#include <new>        
#include <iostream>

/**
 * Constructor
 */
MemoryAllocator::MemoryAllocator(size_t size)
    : pool_start(nullptr),
      total_pool_size(size),
      free_list(nullptr)
{
    pool_start = std::malloc(total_pool_size);
    if (!pool_start) {
        throw std::bad_alloc();
    }

    free_list = static_cast<Block*>(pool_start);
    free_list->size = total_pool_size - sizeof(Block);
    free_list->is_free = true;
    free_list->next = nullptr;
}

/**
 * Destructor
 */
MemoryAllocator::~MemoryAllocator() {
    std::free(pool_start);
}

/**
 * Aligns size to 8 bytes
 */
size_t MemoryAllocator::align(size_t size) {
    return (size + 7) & ~static_cast<size_t>(7);
}

uint8_t* MemoryAllocator::try_allocate(Block* curr, size_t aligned_size) {
    if (curr->is_free && curr->size >= aligned_size) {
        if (curr->size >= aligned_size + sizeof(Block) + 8) {
            uint8_t* base = reinterpret_cast<uint8_t*>(curr);
            Block* new_block = reinterpret_cast<Block*>(base + sizeof(Block) + aligned_size);

            new_block->size = curr->size - aligned_size - sizeof(Block);
            new_block->is_free = true;
            new_block->next = curr->next;

            curr->size = aligned_size;
            curr->next = new_block;
        }

        curr->is_free = false;
        return reinterpret_cast<uint8_t*>(curr) + sizeof(Block);
    }
    return nullptr;
}

/**
 * Allocate memory
 */
void* MemoryAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(allocation_mutex);

    size_t aligned_size = align(size);
    Block* curr = free_list;

    while (curr) {
        uint8_t* ptr = try_allocate(curr, aligned_size);
        if (ptr != nullptr){
            return ptr;
        }
        curr = curr->next;
    }
    // If failed, coalesce and try once more
    coalesce();
    curr = free_list; // Reset to start
    while (curr) {
        uint8_t* ptr = try_allocate(curr, aligned_size);
        if (ptr != nullptr) {
            return ptr;
        }
        curr = curr->next;
    }
    return nullptr;
}

/**
 * Free memory
 */
void MemoryAllocator::deallocate(void* ptr) {
    if (!ptr) return;

    std::lock_guard<std::mutex> lock(allocation_mutex);

    Block* block =
        reinterpret_cast<Block*>(
            reinterpret_cast<uint8_t*>(ptr) - sizeof(Block)
        );

    block->is_free = true;
}

/**
 * Merge adjacent free blocks
 */
void MemoryAllocator::coalesce() {
    Block* curr = free_list;

    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += curr->next->size + sizeof(Block);
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

/**
 * Debug heap dump
 */
void MemoryAllocator::dump_heap() {
    std::lock_guard<std::mutex> lock(allocation_mutex);

    Block* curr = free_list;
    std::cout << "--- Memory Pool State ---\n";
    while (curr) {
        std::cout
            << "[Addr: " << curr
            << " | Size: " << curr->size
            << " | " << (curr->is_free ? "FREE" : "USED")
            << "] -> ";
        curr = curr->next;
    }
    std::cout << "NULL\n";
}
