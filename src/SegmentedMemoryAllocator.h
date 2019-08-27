#ifndef SEGMENTED_MEMORY_ALLOCATOR_H
#define SEGMENTED_MEMORY_ALLOCATOR_H

#include<unistd.h>
#include <list>

#define CURRENT 0
#define EXCEEDED -1

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

    void* allocFirstFit(size_t size);
    void* allocBestFit(size_t size);
    void* allocWorstFit(size_t size);
    void dealloc(void* memoryAddress);
    
    void printAllocatedList();
    void printDeallocatedList();

private:

    block* createBlock(size_t size);

    list<block*>* allocatedMemory;
    list<block*>* deallocatedMemory;
};

#endif // SEGMENTED_MEMORY_ALLOCATOR_H