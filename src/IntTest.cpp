#include <iostream>
#include <chrono>
#include <vector>
#include <sys/resource.h>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::endl;
using std::vector;
using std::clock_t;

#define LOW 7000
#define MEDIUM  20000
#define HIGH    50000
#define INTERVAL    2000

int main() {

    auto begin = std::chrono::high_resolution_clock::now();
    vector<int*>* nums = (vector<int*>*)SegmentedMemoryAllocator::alloc(sizeof(vector<int*>), BEST_FIT);
    struct rusage rUsage;
    long subtract = 0;

    // Create variable-sized free blocks
    for(int i = 0; i < MEDIUM; ++i) {

        size_t size = 0;
        if(i % 2 == 0) {
            size = 5;
        }
        else if(i % 3 == 0) {
            size = 8;
        }
        else if(i % 5 == 0) {
            size = 13;
        }
        else if(i % 7 == 0) {
            size = 21;
        }
        else {
            size = 34;
        }
        SegmentedMemoryAllocator::requestFreeBlock(size);
        if(i % INTERVAL == 0) {
            getrusage(RUSAGE_SELF, &rUsage);
            // Max set size used (in kilobytes)
            printf("generating free block Memory usage = %ld ", rUsage.ru_maxrss);
            if(i == 0) {
                cout << "Time=" << 0 << endl;
                std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                auto current = now.time_since_epoch();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current);
                subtract = duration.count();
            }
            else {
                std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                auto current = now.time_since_epoch();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current);
                long currTime = duration.count();
                cout << "Time=" << currTime - subtract << endl;
            }
        }
    }
    cout << endl;
    
    // First produce allocations
    for(int i = 0; i < MEDIUM; ++i) {

        int* n = (int*)SegmentedMemoryAllocator::alloc(sizeof(int), BEST_FIT);
        // Assign the counter value to the pointer
        *n = i;
        nums->push_back(n);
        // Get memory in use every interval of 25 allocations
        if(i % INTERVAL == 0) {
            getrusage(RUSAGE_SELF, &rUsage);
            // Max set size used (in kilobytes)
            printf("alloc Memory usage = %ld ", rUsage.ru_maxrss);
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto current = now.time_since_epoch();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current);
            long currTime = duration.count();
            cout << "Time=" << currTime - subtract << endl;
        }
    }
    cout << endl;
    // Deallocate
    for(int i = 0; i < MEDIUM; ++i) {

        SegmentedMemoryAllocator::dealloc(nums->at(i));
        if(i % INTERVAL == 0) {
            getrusage(RUSAGE_SELF, &rUsage);
            // Max set size used (in kilobytes)
            printf("dealloc Memory usage = %ld ", rUsage.ru_maxrss);
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto current = now.time_since_epoch();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current);
            long currTime = duration.count();
            cout << "Time=" << currTime - subtract << endl;
        }
    }
    cout << endl;
    auto end = std::chrono::high_resolution_clock::now();
    auto time = end - begin;
    cout << "\nTOTAL ELAPSED TIME = " << time/std::chrono::milliseconds(1) << " milliseconds" << endl;
}