#include <iostream>
#include <stdlib.h> 
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;

#define EMPTY 0

list<block*>* allocatedMemory = new list<block*>();
list<block*>* deallocatedMemory = new list<block*>();

// Allocation strategy set for the allocator
// By default, first fit is used
static strategy allocStrategy = FIRSTFIT;

// Variable to hold locking mechanism
pthread_mutex_t lock; 

// SegmentedMemoryAllocator::SegmentedMemoryAllocator() {
//     cout << "BLOCK SIZE=" << BLOCK_SIZE << " bytes" << endl;
// }

// SegmentedMemoryAllocator::~SegmentedMemoryAllocator() {
//     delete allocatedMemory;
//     delete deallocatedMemory;
// }

// Private helper function to construct a block 
// and return the newly created block's memory address
block* createBlock(size_t size) {

    // Begin memory allocation at the current program break
    block* newBlock = (block*)sbrk(CURRENT);
    
    // Total required memory that needs to be allocated is 
    // (size of block) + (size passed in)
    // Increase the program break to fit this block
    size_t sizeRequired = BLOCK_SIZE + size;
    // Memory address of block
    void* address = (void*)sbrk(CURRENT);
    
    // A block with given size can only be allocated if there is enough space in the heap
    if((void*)sbrk(sizeRequired) != (void*)EXCEEDED) {
        // Memory address of newly created block needs to be offset by
        // the fixed block size
        char* newAddress = static_cast<char *>(address) + BLOCK_SIZE;
        newBlock->size = size;
        newBlock->memoryAddress = newAddress;
        newBlock->isFree = false;
    }
    else {
        newBlock = nullptr;
    }
    return newBlock;
}

// Private helper function to construct a new block 
// and add it to the allocated list only if free list is currently empty OR 
// if no suitable block in the free list could be found.
block* newAlloc(size_t size, bool spaceFound) {
    // Lock the allocated list when inserting a new block   
    pthread_mutex_lock(&lock);
    block* newBlock = nullptr;
    if(!spaceFound) {
        newBlock = createBlock(size);
        if(newBlock != nullptr) {
            allocatedMemory->push_back(newBlock);
        }
    }
    // Unlock the allocated list after a new block was inserted
    pthread_mutex_unlock(&lock); 
    return newBlock;
}

// Return a memory address corresponding to the first block 
// found in the list which is large enough to hold the data
// Returns a null pointer if there's no space left in the heap
//    for the requested size.
void* allocFirstFit(size_t size) {
    // Lock the deallocated list for reading     
    pthread_mutex_lock(&lock);

    block* newBlock = nullptr;
    void* returnAddress = nullptr;
    bool spaceFound = false;
    bool blockSplit = false;
    // Traverse all blocks in free list until the first 
    // suitable free space is found
    if(deallocatedMemory->size() > EMPTY) {
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end() && !spaceFound; ++it) 
        {
            // If a space large enough was found
            if((*it)->size >= size && ((*it)->isFree)) {
                spaceFound = true;
                
                // Split existing free block if its available size 
                // is greater than the required size
                if((*it)->size > size) {
                    size_t remainder = ((*it)->size) - size;
                    // Create remainder block and add to end of allocated list
                    newBlock = createBlock(remainder);
                    // If a remainder block was constructed, 
                    // add to back of free list
                    if(newBlock != nullptr) {
                        newBlock->size = remainder;
                        newBlock->isFree = true;
                        deallocatedMemory->push_back(newBlock);
                        blockSplit = true;
                    }
                }
                (*it)->size = size;
                (*it)->isFree = false;
                // Transfer the now allocated block 
                // from free list to allocated list
                allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
                returnAddress = (*it)->memoryAddress;
            }
        }
    }
    if(deallocatedMemory->size() == EMPTY || !spaceFound) {
        newBlock = newAlloc(size, spaceFound);
    }
    if(newBlock != nullptr && !blockSplit) {
        returnAddress = newBlock->memoryAddress;
    }
    // Unlock the deallocated list
    pthread_mutex_unlock(&lock); 

    return returnAddress;
}

// Return a memory address corresponding to the closest fitting block 
// found in the list which is large enough to hold the data
// Returns a null pointer if there's no space left in the heap
//    for the requested size.
void* allocBestFit(size_t size) {
    // Lock the deallocated list for reading     
    pthread_mutex_lock(&lock);

    block* newBlock = nullptr;
    void* returnAddress = nullptr;
    bool spaceFound = false;
    bool blockSplit = false;
    if(deallocatedMemory->size() > EMPTY) {
        // Find the smallest suitable block for requested size,
        // starting at the head of free list
        block* targetBlock = nullptr;
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end(); ++it) 
        {
            // If a space large enough was found
            if((*it)->size >= size && ((*it)->isFree)) {
                if(targetBlock == nullptr || ((*it)->size <= targetBlock->size && ((*it)->isFree))) {
                    targetBlock = (*it);
                }
            }  
        }
        if(targetBlock != nullptr) {
            if(targetBlock->size >= size) {
                // Split existing free block if its available size 
                // is greater than the required size
                if(targetBlock->size > size) {
                    size_t remainder = (targetBlock->size) - size;
                    // Create remainder block and add to end of allocated list
                    newBlock = createBlock(remainder);
                    // If a remainder block was constructed, 
                    // add to back of free list
                    if(newBlock != nullptr) {
                        newBlock->size = remainder;
                        newBlock->isFree = true;
                        deallocatedMemory->push_back(newBlock);
                        blockSplit = true;
                    }
                }
                list<block*>::iterator it;
                // Find position of smallest suitable block in free list
                for(it = deallocatedMemory->begin(); 
                    it != deallocatedMemory->end() && !spaceFound; ++it)
                {
                    if((*it)->memoryAddress == targetBlock->memoryAddress) {
                        spaceFound = true;
                        (*it)->size = size;
                        (*it)->isFree = false;
                        // Transfer the now allocated block 
                        // from free list to allocated list
                        allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
                        returnAddress = (*it)->memoryAddress;
                    }
                }
            }
        }
    }
    if(deallocatedMemory->size() == EMPTY || !spaceFound) {
        newBlock = newAlloc(size, spaceFound);
    }
    if(newBlock != nullptr && !blockSplit) {
        returnAddress = newBlock->memoryAddress;
    }
    // Unlock the deallocated list
    pthread_mutex_unlock(&lock); 
    return returnAddress;
}

// Return a memory address corresponding to the largest available 
// block found in the list which is large enough to hold the data
// Returns a null pointer if there's no space left in the heap
//    for the requested size.
void* allocWorstFit(size_t size) {
    // Lock the deallocated list for reading     
    pthread_mutex_lock(&lock);

    block* newBlock = nullptr;
    void* returnAddress = nullptr;
    bool spaceFound = false;
    bool blockSplit = false;
    if(deallocatedMemory->size() > EMPTY) {
        // Find the largest suitable block for requested size,
        // starting at the head of free list
        block* targetBlock = nullptr;
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end(); ++it) 
        {
            // If a space large enough was found
            if((*it)->size > size && ((*it)->isFree)) {
                if(targetBlock == nullptr) {
                    targetBlock = (*it);
                }
                if((*it)->size >= targetBlock->size && ((*it)->isFree)) {
                    targetBlock = (*it);
                }
            }  
        }
        if(targetBlock != nullptr) {
            if(targetBlock->size >= size) {
                // Split existing free block if its available size 
                // is greater than the required size
                if(targetBlock->size > size) {
                    size_t remainder = (targetBlock->size) - size;
                    // Create remainder block and add to end of allocated list
                    newBlock = createBlock(remainder);
                    // If a remainder block was constructed, 
                    // add to back of free list
                    if(newBlock != nullptr) {
                        newBlock->size = remainder;
                        newBlock->isFree = true;
                        deallocatedMemory->push_back(newBlock);
                        blockSplit = true;
                    }
                }
                list<block*>::iterator it;
                // Find position of largest suitable block in free list
                for(it = deallocatedMemory->begin(); 
                    it != deallocatedMemory->end() && !spaceFound; ++it)
                {
                    if((*it)->memoryAddress == targetBlock->memoryAddress) {
                        spaceFound = true;
                        (*it)->size = size;
                        (*it)->isFree = false;
                        // Transfer the now allocated block 
                        // from free list to allocated list
                        allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
                        returnAddress = (*it)->memoryAddress;
                    }
                }
            }
        }
    }
    if(deallocatedMemory->size() == EMPTY || !spaceFound) {
        newBlock = newAlloc(size, spaceFound);
    }
    if(newBlock != nullptr && !blockSplit) {
        returnAddress = newBlock->memoryAddress;
    }
    // Unlock the deallocated list
    pthread_mutex_unlock(&lock); 
    return returnAddress;
}

// Primary alloc function called from main with a specified
// strategy for block allocation.
// First Fit Strategy 0
// Best Fit Strategy 1
// Worst Fit Strategy 2
void* alloc(size_t size) {
    void* memoryAddress = nullptr;

    if(allocStrategy == FIRSTFIT) {
        memoryAddress = allocFirstFit(size);
    }
    else if(allocStrategy == BESTFIT) {
        memoryAddress = allocBestFit(size);
    }
    else if(allocStrategy == WORSTFIT) {
        memoryAddress = allocWorstFit(size);
    }
    return memoryAddress;
}

// Primary dealloc function called from main to free a
// block in the allocated list
void dealloc(void* memoryAddress) {
    // Lock the deallocated list for reading     
    pthread_mutex_lock(&lock);

    bool blockFound = false;
    list<block*>::iterator it;
    for (it = allocatedMemory->begin(); 
                it != allocatedMemory->end() && !blockFound; ++it) 
    {
        if((*it)->memoryAddress == memoryAddress) {
            blockFound = true;
            (*it)->isFree = true;
            // Move the block found to the free list
            // and remove the block from the allocated list
            deallocatedMemory->splice(deallocatedMemory->end(), *allocatedMemory, it);
        }
    }
    if(!blockFound) {
        cout << "could not find memory address " << memoryAddress << 
            "\nTerminated program." << endl;
        // Terminate program
        abort();
    }
    // Unlock the deallocated list
    pthread_mutex_unlock(&lock); 
}

// Test function to construct 'free' blocks and add
// them to the free list
// Delete later
void requestFreeBlock(size_t size) {
    block* freeBlock = createBlock(size);
    if(freeBlock != nullptr) {
        freeBlock->isFree = true;
        deallocatedMemory->push_back(freeBlock);
    }
}

// Print all memory blocks that HAVE NOT been freed, sequentially
void printAllocatedList() {
    for (list<block*>::iterator it = allocatedMemory->begin(); 
                it != allocatedMemory->end(); ++it) 
    {
        cout << "(" << (*it)->memoryAddress << " | " << (*it)->size << 
            " bytes | " << (*it)->isFree << ") -> ";
    }
}

// Print all memory blocks that HAVE been freed, sequentially
void printDeallocatedList() {
    for (list<block*>::iterator it = deallocatedMemory->begin(); 
                it != deallocatedMemory->end(); ++it) 
    {
        cout << "(" << (*it)->memoryAddress << " | " << (*it)->size << 
            " bytes | " << (*it)->isFree << ") -> ";
    }
}

void setStrategy(strategy newStrategy) {
    allocStrategy = newStrategy;
}