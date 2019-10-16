#ifndef SEGMENTED_MEMORY_ALLOCATOR_H
#define SEGMENTED_MEMORY_ALLOCATOR_H

#include <unistd.h>
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

enum strategy
{
    FIRSTFIT,
    BESTFIT,
    WORSTFIT
};


using std::list;

void* alloc(size_t size);
void dealloc(void* memoryAddress);
    
void printAllocatedList();
void printDeallocatedList();

// dummy test function, remove later
void requestFreeBlock(size_t size);

// Change the allocation strategy in use
void setStrategy(strategy newStrategy);

block* createBlock(size_t size);
block* newAlloc(size_t size, bool spaceFound);

void* allocFirstFit(size_t size);
void* allocBestFit(size_t size);
void* allocWorstFit(size_t size);

#endif // SEGMENTED_MEMORY_ALLOCATOR_H