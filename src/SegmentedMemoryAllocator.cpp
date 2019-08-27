#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;

#define EMPTY 0

SegmentedMemoryAllocator::SegmentedMemoryAllocator() :
    allocatedMemory(new list<block*>()),
    deallocatedMemory(new list<block*>())
{
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
    newBlock = this->newAlloc(size, spaceFound);
    if(newBlock != nullptr) {
        returnAddress = newBlock->memoryAddress;
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return returnAddress;
}

// Best fit: iterate through ALL block nodes and determine the node 
//      with the closest size value to the size being inserted.
//      Out of all the blocks which are able to satisfy the size
//      required, pick the smallest one.
void* SegmentedMemoryAllocator::allocBestFit(size_t size) {
    block* newBlock = nullptr;
    void* returnAddress = nullptr;

    bool spaceFound = false;
    // Traverse all blocks in free list until the first 
    // suitable free space is found
    if(deallocatedMemory->size() > EMPTY) {
        // Find the smallest suitable block for requested size,
        // starting at the head of free list
        size_t smallestFit = deallocatedMemory->front()->size;

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
    newBlock = this->newAlloc(size, spaceFound);
    if(newBlock != nullptr) {
        returnAddress = newBlock->memoryAddress;
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return returnAddress;
}

// // Worst fit: iterate through ALL block nodes and determine the node 
// //      with the largest size value, split, then create a rem block 
// //      and push back to deallocated list.
// //      Out of all the blocks which are able to satisfy the size
// //      required, pick the largest one.
// void* SegmentedMemoryAllocator::allocWorstFit(size_t size) {

// }

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
    cout << "dealloc successful.\nsize of free list is now " 
        << deallocatedMemory->size() << "\nsize of allocated list is now " 
        << allocatedMemory->size() << endl;
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


    


