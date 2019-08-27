#ifndef SEGMENTED_MEMORY_ALLOCATOR_H
#define SEGMENTED_MEMORY_ALLOCATOR_H

#include<unistd.h>
#include <list>

#define CURRENT 0
#define EXCEEDED -1

#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3

// the size of the block structure itself (including info)
#define BLOCK_SIZE sizeof(block) // in bytes

// Represents a block of memory in the heap
struct block {
    size_t size;
    // start of the memory address for which the block is allocated
    void* memoryAddress;
    // True if the current block is free, false otherwise
    bool isFree;
};

using std::list;

class SegmentedMemoryAllocator {
public:

    SegmentedMemoryAllocator();
    ~SegmentedMemoryAllocator();

    static void* alloc(size_t size, int strategy);
    static void dealloc(void* memoryAddress);
    
    static void printAllocatedList();
    static void printDeallocatedList();

    // dummy test function, remove later
    static void requestFreeBlock(size_t size);

private:

    static block* createBlock(size_t size);
    static block* newAlloc(size_t size, bool spaceFound);
    
    static void* allocFirstFit(size_t size);
    static void* allocBestFit(size_t size);
    static void* allocWorstFit(size_t size);
};

#endif // SEGMENTED_MEMORY_ALLOCATOR_H