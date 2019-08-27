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
