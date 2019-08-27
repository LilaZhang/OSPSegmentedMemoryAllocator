#include <iostream>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;


int main() {

    SegmentedMemoryAllocator* sma = new SegmentedMemoryAllocator();

    int* i = (int*)sma->allocFirstFit(10);
    int* j = (int*)sma->allocFirstFit(20);
    int* k = (int*)sma->allocFirstFit(13);
    cout << "int i mem addr=" << i << endl;
    cout << "int j mem addr=" << j << endl;
    cout << "int k mem addr=" << k << endl;

    sma->dealloc(k);

    sma->requestFreeBlock(80);
    int* m = (int*)sma->allocFirstFit(50);
    cout << "int m mem addr=" << m << endl;

    cout << "\nALLOCATED LIST:" << endl;
    sma->printAllocatedList();
    cout << "\n\nFREE LIST:" << endl;
    sma->printDeallocatedList();
}