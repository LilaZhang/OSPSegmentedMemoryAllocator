#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;


int main() {

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

    // Best fit tests
    SegmentedMemoryAllocator::requestFreeBlock(30);
    SegmentedMemoryAllocator::requestFreeBlock(15);
    SegmentedMemoryAllocator::requestFreeBlock(45);

    int* l = (int*)SegmentedMemoryAllocator::alloc(14, 3);
    cout << "int l mem addr=" << l << endl;

    cout << "\nALLOCATED LIST:" << endl;
    SegmentedMemoryAllocator::printAllocatedList();
    cout << "\n\nFREE LIST:" << endl;
    SegmentedMemoryAllocator::printDeallocatedList();
}