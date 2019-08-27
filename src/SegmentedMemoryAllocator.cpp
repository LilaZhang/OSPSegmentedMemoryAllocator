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
    cout << "#~ block size=" << BLOCK_SIZE << " bytes ~#" << endl;
}

SegmentedMemoryAllocator::~SegmentedMemoryAllocator() {
    delete allocatedMemory;
    delete deallocatedMemory;
}

// Private helper method to construct a block 
// and return its memory address
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

// Return a memory address corresponding to the first block found in the list 
// that is large enough to hold the data
void* SegmentedMemoryAllocator::allocFirstFit(size_t size) {
    
    block* newBlock = createBlock(size);
    block* remBlock = nullptr;

    if(newBlock != nullptr) {
        // If free list is empty, no valid blocks are available
        // Add to allocated list without traversing free list
        if(deallocatedMemory->size() == EMPTY) {
            cout << "free list currently empty" << endl;
            allocatedMemory->push_back(newBlock);
        }
        // Otherwise traverse all blocks in free list until the first 
        // free space large enough is found
        else {
            bool spaceFound = false;
            for (list<block*>::iterator it = deallocatedMemory->begin(); 
                it != deallocatedMemory->end() && !spaceFound; ++it) 
            {
                cout << "(current size=" << (*it)->size << " bytes, isFree=" << (*it)->isFree << ")" << endl;
                // If a space large enough was found
                if((*it)->size >= size && ((*it)->isFree)) {
                    cout << "found a space in free list!" << endl;
                    spaceFound = true;
                            
                    // Split block if available size is greater than required size
                    if((*it)->size > size) {
                        cout << "splitting free block..." << endl;
                        // Allocate newBlock to the free suitable block
                        size_t remainder = ((*it)->size) - size;
                        (*it)->size = newBlock->size;
                        (*it)->memoryAddress = newBlock->memoryAddress;
                        (*it)->isFree = false;

                        // Create remainder block and add to end of list
                        cout << "creating remainder block of size=" << remainder << endl;
                        remBlock = createBlock(remainder);
                    }
                }
            }
            // If no suitable free block was found, 
            // add newBlock to end of allocatedMemory list
            if(!spaceFound) {
                allocatedMemory->push_back(newBlock);
            }
        } 
    }
    // If there's no available memory left in the heap, 
    // return an empty pointer
    else {
        newBlock = nullptr;
    }

    // If a remainder block was constructed, 
    // add to back of free list
    if(remBlock != nullptr) {
        deallocatedMemory->push_back(remBlock);
    }
    cout << "size of allocated list is now " << allocatedMemory->size() << endl;
    return newBlock->memoryAddress;
}

// // Best fit: iterate through ALL block nodes and determine the node 
// //      with the closest size value to the size being inserted.
// //      Out of all the blocks which are able to satisfy the size
// //      required, pick the smallest one.
// void* SegmentedMemoryAllocator::allocBestFit(size_t size) {

// }

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
            cout << "found the block with mem addr to be freed.\n at " 
                << (*it)->memoryAddress << endl;
            blockFound = true;
            // Move the block found to the free list
            // and remove the block from the allocated list
            deallocatedMemory->splice(deallocatedMemory->end(), *allocatedMemory, it);
        }
    }
    cout << "dealloc successful.\nsize of free list is now " 
        << deallocatedMemory->size() << "\nsize of allocated list is now " 
        << allocatedMemory->size() << endl;
}

// Print all memory blocks that HAVE NOT been freed, sequentially
void SegmentedMemoryAllocator::printAllocatedList() {

}

// Print all memory blocks that HAVE been freed, sequentially
void SegmentedMemoryAllocator::printDeallocatedList() {

}


    


