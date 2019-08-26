// Return a memory address corresponding to the first block found in the list 
// that is large enough to hold the data
void* SegmentedMemoryAllocator::allocFirstFit(size_t size) {
    // begin memory allocation at the current program break
    block* newBlock = (block*)sbrk(CURRENT);
    block* remBlock = nullptr;
    
    // total required memory that needs to be allocated is (size of block) + (size passed in)
    // increase the program break to fit this block
    size_t sizeRequired = BLOCK_SIZE + size;
    // memory address of block
    void* address = (void*)sbrk(CURRENT);

    // should be the same
    cout << "newBlock addr=" << newBlock << endl;
    cout << "address=" << address << endl;
    
    // a block with given size can only be allocated if there is enough space in the heap
    if((void*)sbrk(sizeRequired) != (void*)EXCEEDED) {
        
        char* newAddress = static_cast<char *>(address) + BLOCK_SIZE;
        newBlock->size = size;
        newBlock->memoryAddress = newAddress;
        newBlock->isFree = false;

        // if empty list, set head to new block created
        if(allocatedMemory->size() == 0) {
            allocatedMemory->push_front(newBlock);
        }
        else {

            bool spaceFound = false;
            for (std::list<block*>::iterator it = allocatedMemory->begin(); 
                it != allocatedMemory->end(); ++it) 
            {
                
                if(!spaceFound) {

                    cout << "current size=" << (*it)->size << " bytes, isFree=" << (*it)->isFree << endl;
                    // If a space large enough was found
                    if((*it)->size >= size && ((*it)->isFree)) {
                        spaceFound = true;
                        
                        // Split block if available size is greater than required size
                        if((*it)->size > size) {
                            // Allocate newBlock to the free suitable block
                            size_t remainder = ((*it)->size) - size;
                            (*it)->size = newBlock->size;
                            (*it)->memoryAddress = newBlock->memoryAddress;
                            (*it)->isFree = false;

                            // Create remainder block and add to end of list
                            remBlock = (block*)sbrk(CURRENT);
                            size_t sizeRequiredRem = BLOCK_SIZE + remainder;
                            // memory address of block
                            void* remAddress = (void*)sbrk(CURRENT);

                            // should be the same
                            cout << "remBlock addr=" << remBlock << endl;
                            cout << "remAddress=" << remAddress << endl;
                            
                            if((void*)sbrk(sizeRequiredRem) != (void*)EXCEEDED) {
                                char* newRemAddress = static_cast<char *>(remAddress) + BLOCK_SIZE;
                                remBlock->size = remainder;
                                remBlock->memoryAddress = newRemAddress;
                                remBlock->isFree = true;
                            }
                        }
                    }
                }
            }
            // If no suitable free block was found
            if(!spaceFound) {
                allocatedMemory->push_back(newBlock);
            }
        } 
    }
    // If there's no available memory left in the heap, return empty pointer
    else {
        newBlock = nullptr;
    }
    if(remBlock != nullptr) {
        allocatedMemory->push_back(remBlock);
    }
    return address;
}