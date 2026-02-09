#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <cstddef>   // size_t
#include <mutex>

/**
 * Custom Memory Allocator - Free List Implementation
 * Manages a pre-allocated "Arena" of memory to avoid frequent OS calls.
 */
class MemoryAllocator {
public:
    /**
     * Constructor initializes the memory pool.
     * @param size The total size of the memory pool in bytes.
     */
    explicit MemoryAllocator(size_t size);

    /**
     * Destructor frees the memory pool.
     */
    ~MemoryAllocator();

    /**
     * Allocates a block of memory from the pool.
     * @param size Number of bytes to allocate.
     * @return Pointer to the allocated memory, or nullptr if allocation fails.
     */
    void* allocate(size_t size);

    /**
     * Returns a block of memory to the pool.
     * @param ptr Pointer to the block to be freed.
     */
    void deallocate(void* ptr);

    void dump_heap();

private:
    struct Block {
        size_t size;
        bool is_free;
        Block* next;
    };

    void* pool_start;
    size_t total_pool_size;
    Block* free_list;
    std::mutex allocation_mutex;

    size_t align(size_t size);
    void coalesce();
    uint8_t* try_allocate(Block* curr, size_t aligned_size);
    
};

#endif // MEMORY_ALLOCATOR_H
