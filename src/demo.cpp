#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;

int main() {

    cout << "Segmented Memory Allocator\n============================\n" << 
    "Commands:\na     alloc(size_t size, int strategy)\n" << 
    "d     dealloc(void* memoryAddress)" << endl;

    int strategy = FIRST_FIT;
    // First fit tests
    // int* i = (int*)SegmentedMemoryAllocator::alloc(10, 1);
    // int* j = (int*)SegmentedMemoryAllocator::alloc(20, 1);
    // int* k = (int*)SegmentedMemoryAllocator::alloc(13, 1);
    // cout << "int i mem addr=" << i << endl;
    // cout << "int j mem addr=" << j << endl;
    // cout << "int k mem addr=" << k << endl;

    // SegmentedMemoryAllocator::dealloc(k);

    // SegmentedMemoryAllocator::requestFreeBlock(80);
    // int* m = (int*)SegmentedMemoryAllocator::alloc(50, 1);
    // cout << "int m mem addr=" << m << endl;

    // cout << "Which allocator?\n1   First-fit\n2   Best-fit\n3   Worst fit" << endl;
    // int choice = 0;
    // cin >> choice;

    // Best fit tests
    SegmentedMemoryAllocator::requestFreeBlock(30);
    SegmentedMemoryAllocator::requestFreeBlock(15);
    SegmentedMemoryAllocator::requestFreeBlock(45);

    int* l = (int*)SegmentedMemoryAllocator::alloc(sizeof(int), strategy);
    cout << "int l mem addr=" << l << endl;

    int* s = new int(5);
    SegmentedMemoryAllocator::dealloc(s);

    cout << "\nALLOCATED LIST:" << endl;
    SegmentedMemoryAllocator::printAllocatedList();
    cout << "\n\nFREE LIST:" << endl;
    SegmentedMemoryAllocator::printDeallocatedList();
}