#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;

#define EMPTY 0

list<block*>* allocatedMemory = new list<block*>();
list<block*>* deallocatedMemory = new list<block*>();

SegmentedMemoryAllocator::SegmentedMemoryAllocator() {
    cout << "BLOCK SIZE=" << BLOCK_SIZE << " bytes" << endl;
}

SegmentedMemoryAllocator::~SegmentedMemoryAllocator() {
    delete allocatedMemory;
    delete deallocatedMemory;
}

// Private helper function to construct a block 
// and return the newly created block's memory address
block* SegmentedMemoryAllocator::createBlock(size_t size) {

    // Begin memory allocation at the current program break
    block* newBlock = (block*)sbrk(CURRENT);
    
    // Total required memory that needs to be allocated is 
    // (size of block) + (size passed in)
    // Increase the program break to fit this block
    size_t sizeRequired = BLOCK_SIZE + size;
    cout << "size required=" << sizeRequired << endl;
    // Memory address of block
    void* address = (void*)sbrk(CURRENT);

    // Should be the same
    cout << "\nnewBlock addr=" << newBlock << endl;
    cout << "address=" << address << endl;
    
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
block* SegmentedMemoryAllocator::newAlloc(size_t size, bool spaceFound) {
    block* newBlock = nullptr;
    if(!spaceFound || deallocatedMemory->size() == EMPTY) {
        // Debug messages, delete later
        if(!spaceFound) {
            cout << "no suitable block was found." << endl;
        }
        else if(deallocatedMemory->size() == EMPTY) {
            cout << "free list currently empty." << endl;
        }

        newBlock = createBlock(size);
        if(newBlock != nullptr) {
            allocatedMemory->push_back(newBlock);
            cout << "Created a new block." << endl;
        }
    }
    return newBlock;
}

// Return a memory address corresponding to the first block 
// found in the list which is large enough to hold the data
// Returns a null pointer if there's no space left in the heap
//    for the requested size.
void* SegmentedMemoryAllocator::allocFirstFit(size_t size) {
        
    block* newBlock = nullptr;
    void* returnAddress = nullptr;

    bool spaceFound = false;
    // Traverse all blocks in free list until the first 
    // suitable free space is found
    if(deallocatedMemory->size() > EMPTY) {
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end() && !spaceFound; ++it) 
        {
            cout << "(current size=" << (*it)->size << " bytes, isFree=" 
                << (*it)->isFree << ")" << endl;
            // If a space large enough was found
            if((*it)->size >= size && ((*it)->isFree)) {
                cout << "found a space in free list!" << endl;
                spaceFound = true;
                
                // Split existing free block if its available size 
                // is greater than the required size
                if((*it)->size > size) {
                    cout << "creating a split block.." << endl;
                    size_t remainder = ((*it)->size) - size;
                    // Create remainder block and add to end of allocated list
                    newBlock = createBlock(remainder);
                    // If a remainder block was constructed, 
                    // add to back of free list
                    if(newBlock != nullptr) {
                        cout << "created remainder block of size=" << remainder << endl;
                        newBlock->size = remainder;
                        newBlock->isFree = true;
                        deallocatedMemory->push_back(newBlock);
                        cout << "free block split." << endl;
                    }
                }
                (*it)->size = size;
                (*it)->isFree = false;
                // Transfer the now allocated block 
                // from free list to allocated list
                allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
            }
        }
    }
    newBlock = newAlloc(size, spaceFound);
    if(newBlock != nullptr) {
        returnAddress = newBlock->memoryAddress;
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return returnAddress;
}

// Return a memory address corresponding to the smallest
// block capable of holding the requested size
void* SegmentedMemoryAllocator::allocBestFit(size_t size) {
    block* newBlock = nullptr;
    void* returnAddress = nullptr;

    bool spaceFound = false;
    if(deallocatedMemory->size() > EMPTY) {
        // Find the smallest suitable block for requested size,
        // starting at the head of free list
        block* smallestBlockFit = deallocatedMemory->front();
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end(); ++it) 
        {
            cout << "(current size=" << (*it)->size << " bytes, isFree=" 
                << (*it)->isFree << ")" << endl;
            // If a space large enough was found
            if((*it)->size >= size && ((*it)->isFree)) {
                cout << "found a space in free list!" << endl;

                if((*it)->size <= smallestBlockFit->size && ((*it)->isFree)) {
                    smallestBlockFit = (*it);
                }
            }  
        }
        cout << "smallest fit block has size=" << smallestBlockFit->size << " bytes." << endl;
        if(smallestBlockFit->size >= size) {
            // Split existing free block if its available size 
            // is greater than the required size
            if(smallestBlockFit->size > size) {
                cout << "creating a split block.." << endl;
                size_t remainder = (smallestBlockFit->size) - size;
                // Create remainder block and add to end of allocated list
                newBlock = createBlock(remainder);
                // If a remainder block was constructed, 
                // add to back of free list
                if(newBlock != nullptr) {
                    cout << "created remainder block of size=" << remainder << endl;
                    newBlock->size = remainder;
                    newBlock->isFree = true;
                    deallocatedMemory->push_back(newBlock);
                    cout << "block split successful." << endl;
                }
            }
            list<block*>::iterator it;
            // Find position of smallest suitable block in free list
            for(it = deallocatedMemory->begin(); 
                it != deallocatedMemory->end() && !spaceFound; ++it)
            {
                if((*it)->memoryAddress == smallestBlockFit->memoryAddress) {
                    spaceFound = true;
                    (*it)->size = size;
                    (*it)->isFree = false;
                    // Transfer the now allocated block 
                    // from free list to allocated list
                    allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
                }
            }
        }
    }
    newBlock = newAlloc(size, spaceFound);
    if(newBlock != nullptr) {
        returnAddress = newBlock->memoryAddress;
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return returnAddress;
}

// Return a memory address corresponding to the largest
// block capable of holding the requested size
void* SegmentedMemoryAllocator::allocWorstFit(size_t size) {
    block* newBlock = nullptr;
    void* returnAddress = nullptr;

    bool spaceFound = false;
    if(deallocatedMemory->size() > EMPTY) {
        // Find the smallest suitable block for requested size,
        // starting at the head of free list
        block* largestBlockFit = deallocatedMemory->front();
        for (list<block*>::iterator it = deallocatedMemory->begin(); 
            it != deallocatedMemory->end(); ++it) 
        {
            cout << "(current size=" << (*it)->size << " bytes, isFree=" 
                << (*it)->isFree << ")" << endl;
            // If a space large enough was found
            if((*it)->size > size && ((*it)->isFree)) {
                cout << "found a space in free list!" << endl;

                if((*it)->size >= largestBlockFit->size && ((*it)->isFree)) {
                    largestBlockFit = (*it);
                }
            }  
        }
        cout << "largest fit block has size=" << largestBlockFit->size << " bytes." << endl;
        if(largestBlockFit->size >= size) {
            // Split existing free block if its available size 
            // is greater than the required size
            if(largestBlockFit->size > size) {
                cout << "creating a split block.." << endl;
                size_t remainder = (largestBlockFit->size) - size;
                // Create remainder block and add to end of allocated list
                newBlock = createBlock(remainder);
                // If a remainder block was constructed, 
                // add to back of free list
                if(newBlock != nullptr) {
                    cout << "created remainder block of size=" << remainder << endl;
                    newBlock->size = remainder;
                    newBlock->isFree = true;
                    deallocatedMemory->push_back(newBlock);
                    cout << "block split successful." << endl;
                }
            }
            list<block*>::iterator it;
            // Find position of smallest suitable block in free list
            for(it = deallocatedMemory->begin(); 
                it != deallocatedMemory->end() && !spaceFound; ++it)
            {
                if((*it)->memoryAddress == largestBlockFit->memoryAddress) {
                    spaceFound = true;
                    (*it)->size = size;
                    (*it)->isFree = false;
                    // Transfer the now allocated block 
                    // from free list to allocated list
                    allocatedMemory->splice(allocatedMemory->end(), *deallocatedMemory, it);
                }
            }
        }
    }
    newBlock = newAlloc(size, spaceFound);
    if(newBlock != nullptr) {
        returnAddress = newBlock->memoryAddress;
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return returnAddress;
}

// Primary alloc function called from main with a specified
// strategy for block allocation.
// First Fit Strategy 0
// Best Fit Strategy 1
// Worst Fit Strategy 2
void* SegmentedMemoryAllocator::alloc(size_t size, int strategy) {
    void* memoryAddress = nullptr;

    if(strategy == FIRST_FIT) {
        memoryAddress = allocFirstFit(size);
    }
    else if(strategy == BEST_FIT) {
        memoryAddress = allocBestFit(size);
    }
    else if(strategy == WORST_FIT) {
        memoryAddress = allocWorstFit(size);
    }
    return memoryAddress;
}

// Primary dealloc function called from main to free a
// block in the allocated list
void SegmentedMemoryAllocator::dealloc(void* memoryAddress) {
    cout << "mem addr to search=" << memoryAddress << endl;
    bool blockFound = false;
    list<block*>::iterator it;
    for (it = allocatedMemory->begin(); 
                it != allocatedMemory->end() && !blockFound; ++it) 
    {
        if((*it)->memoryAddress == memoryAddress) {
            cout << "found the block with mem addr to be freed.\nat " 
                << (*it)->memoryAddress << endl;
            blockFound = true;
            (*it)->isFree = true;
            // Move the block found to the free list
            // and remove the block from the allocated list
            deallocatedMemory->splice(deallocatedMemory->end(), *allocatedMemory, it);
        }
    }
    if(blockFound) {
        cout << "dealloc successful.\nsize of free list is now " 
        << deallocatedMemory->size() << "\nsize of allocated list is now " 
        << allocatedMemory->size() << endl;
    }
    else {
        cout << "could not find memory address " << memoryAddress << 
            "\nTerminated program." << endl;
        // Terminate program
        exit(EXIT_FAILURE);
    }
}

// Test function to construct 'free' blocks and add
// them to the free list
// Delete later
void SegmentedMemoryAllocator::requestFreeBlock(size_t size) {
    block* freeBlock = createBlock(size);
    if(freeBlock != nullptr) {
        freeBlock->isFree = true;
        deallocatedMemory->push_back(freeBlock);
    }
}

// Print all memory blocks that HAVE NOT been freed, sequentially
void SegmentedMemoryAllocator::printAllocatedList() {
    for (list<block*>::iterator it = allocatedMemory->begin(); 
                it != allocatedMemory->end(); ++it) 
    {
        cout << "(" << (*it)->memoryAddress << " | " << (*it)->size << 
            " bytes | " << (*it)->isFree << ") -> ";
    }
}

// Print all memory blocks that HAVE been freed, sequentially
void SegmentedMemoryAllocator::printDeallocatedList() {
    for (list<block*>::iterator it = deallocatedMemory->begin(); 
                it != deallocatedMemory->end(); ++it) 
    {
        cout << "(" << (*it)->memoryAddress << " | " << (*it)->size << 
            " bytes | " << (*it)->isFree << ") -> ";
    }
}


    


