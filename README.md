A custom thread-safe memory allocator implemented in C++17 using a free-list strategy over a pre-allocated arena.
Designed to reduce OS allocation overhead and demonstrate low-level memory management concepts.

Execution and Performance Analysis
Build Instructions
The project uses a standard C++17 toolchain to ensure compatibility with modern system standards. To compile the memory manager along with the test suite, use a C++ compiler (such as g++ or clang):

Bash
g++ -std=c++17 main.cpp MemoryAllocator.cpp -o allocator_test -lpthread

-std=c++17: Ensures support for modern language features like std::lock_guard.

-lpthread: Links the POSIX threads library to support the thread-safe std::mutex implementation.

Runtime Lifecycle
The execution follows a deterministic lifecycle designed for high-performance environments:

Arena Initialization: Upon instantiation, a contiguous block of memory (the "Arena") is reserved from the OS using malloc. This reduces the overhead of frequent system calls during runtime.

Request & Alignment: When allocate(size_t) is called, the requested size is bitwise-aligned to an 8-byte boundary to ensure optimal CPU cache performance and hardware compatibility.

Search Strategy: The allocator employs a First-Fit strategy to traverse the free list, identifying the first available block that meets the size requirement.

Block Splitting: If a block is significantly larger than the request, it is dynamically split into two: a "Used" block for the caller and a new "Free" block for future use.

Thread Safety: Every allocation and deallocation is guarded by a mutex, preventing race conditions in multi-threaded applications.

Coalescing: During deallocate(), the manager automatically performs a coalescing pass, merging adjacent free blocks to mitigate external fragmentation and maintain heap health.

Verifying the Output
The included dump_heap() utility provides a visual trace of the memory state. In a successful execution, you will observe the initial large pool being partitioned and subsequently merged back into a single contiguous block after all memory is freed, demonstrating the integrity of the pointer arithmetic and the efficiency of the coalescing algorithm.
