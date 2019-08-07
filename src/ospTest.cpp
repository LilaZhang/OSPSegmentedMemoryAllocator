#include <iostream>
#include<unistd.h>

using std::cout;
using std::cin;
using std::endl;

typedef struct blockStrc {
    
    // size of the block allocated in memory
    size_t size;
    
    // True if the current block is free, false otherwise
    bool isFree;
    
    // start of the memory address for which the block is allocated
    void* memoryAddress;
    
    // pointer to the next memory block
    struct blockStrc* next;
    
}MemBlock;

#define CURRENT 0

// the size of the block structure itself (along with all its attributes)
#define SIZE sizeof(MemBlock) // in bytes

//test only
#define INT_SIZE sizeof(int) // in bytes

// define functions for adding/removing/print to the linked list here

int main() {
    
    // program break = edge of the heap
    
    // how much size we want to allocate in addition to the size of a MemBlock
    //***int size = 10;
    
    // allocate memory for a new block
    // sbrk adds to the program break and returns a pointer to the old edge 
    // set the memory address of the new block to add, to the CURRENT program break
    //***MemBlock *block = (MemBlock*) sbrk(CURRENT);
    
    // void pointer is universal
    //***void *blockMemAddr = (void*) sbrk(CURRENT);
    //***cout << "block's memory address=" << blockMemAddr << endl;
    
    
    // test program to allocate memory in the heap for the first 10 multiples of 3
    
    for(int i = 1; i <= 10; ++i) {
        // set the memory address of the new int to the CURRENT program break
        int *ptr = (int*) sbrk(CURRENT);
        cout << "int's CURRENT memory address=" << ptr << endl;

        // set program break to 0 CURRENTly
        void *intMemAddr = (void*) sbrk(CURRENT);
        cout << "int's memory address=" << intMemAddr << endl;

        // add to the program break: allocate space for an integer which is 4 bytes
        void *allocatedIntMem = (void*) sbrk(INT_SIZE);
        cout << "int's old edge after allocating size=" << allocatedIntMem << endl;
        cout << "int's size=" << INT_SIZE << endl;

        *ptr = i * 3;
        cout << "int's value=" << *ptr << endl;

        cout << "current program break=" << sbrk(0) << endl;
    }
    
    // this is the same as the blockMemAddr CURRENTly
    //***void *allocatedMem = (void*) sbrk(SIZE + size);
    //***cout << "block's old edge after allocating size=" << allocatedMem << endl;
    //***cout << "block's size=" << SIZE + size << endl;
    
//    // then use currEdge to free all memory allocated after the currEdge
//    // brk sets the program break to the address passed in
//    // print the current edge of the heap
//    int currEdge = brk(nullptr);
//    cout << "current heap edge=" << currEdge << endl;
}