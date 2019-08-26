#include <iostream>
#include<unistd.h>

using std::cout;
using std::cin;
using std::endl;

#define CURRENT 0

//test only
#define INT_SIZE sizeof(int) // in bytes

// define functions for adding/removing/print to the linked list here

int main() {
    // test program to allocate memory in the heap for the first 10 multiples of 3
    
    for(int i = 1; i <= 10; ++i) {
        // set the memory address of the new int to the CURRENT program break
        int *ptr = (int*) sbrk(CURRENT);
        cout << "int's CURRENT memory address=" << ptr << endl;

        // get current program break
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
}