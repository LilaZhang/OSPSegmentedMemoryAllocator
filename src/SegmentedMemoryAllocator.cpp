#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;

SegmentedMemoryAllocator::SegmentedMemoryAllocator() :
    allocatedMemory(new list<block*>()),
    deallocatedMemory(new list<block*>())
{
    cout << "block size=" << BLOCK_SIZE << " bytes" << endl;
}

SegmentedMemoryAllocator::~SegmentedMemoryAllocator() {
    delete allocatedMemory;
    delete deallocatedMemory;
}

// Private helper method to construct a block and return its address
block* SegmentedMemoryAllocator::createBlock(size_t size) {

    // Begin memory allocation at the current program break
    block* newBlock = (block*)sbrk(CURRENT);
    
    // Total required memory that needs to be allocated is (size of block) + (size passed in)
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
        // If empty list, set head to new block created
        if(allocatedMemory->size() == 0) {
            cout << "list currently empty" << endl;
            allocatedMemory->push_back(newBlock);
        }
        // Otherwise traverse all blocks in the list until a 
        // free space large enough is found
        else {
            bool spaceFound = false;
            for (std::list<block*>::iterator it = allocatedMemory->begin(); 
                it != allocatedMemory->end() && !spaceFound; ++it) 
            {
                cout << "(current size=" << (*it)->size << " bytes, isFree=" << (*it)->isFree << ")" << endl;
                // If a space large enough was found
                if((*it)->size >= size && ((*it)->isFree)) {
                    cout << "found a space!" << endl;
                    spaceFound = true;
                            
                    // Split block if available size is greater than required size
                    if((*it)->size > size) {
                        cout << "splitting block..." << endl;
                        // Allocate newBlock to the free suitable block
                        size_t remainder = ((*it)->size) - size;
                        (*it)->size = newBlock->size;
                        (*it)->memoryAddress = newBlock->memoryAddress;
                        (*it)->isFree = false;

                        // Create remainder block and add to end of list
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
    // If there's no available memory left in the heap, return empty pointer
    else {
        newBlock = nullptr;
    }

    // If a remainder block was constructed, 
    // add to back of allocatedMemory list
    if(remBlock != nullptr) {
        allocatedMemory->push_back(remBlock);
    }

    return newBlock->memoryAddress;
}


    


