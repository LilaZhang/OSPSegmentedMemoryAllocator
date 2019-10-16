#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <sys/resource.h>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::clock_t;

#define LOW 7000
#define MEDIUM  20000
#define HIGH    50000
#define INTERVAL    2000

int main() {

    auto begin = std::chrono::high_resolution_clock::now();
    vector<char const**>* words = (vector<char const**>*)SegmentedMemoryAllocator::alloc(sizeof(vector<char const**>), FIRST_FIT);
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
    string fileName = "Collins Scrabble Words (2015)";
    // Read in input data file
    std::ifstream inFile;
    inFile.open(fileName + ".txt");
    int count = MEDIUM;
    
    // First produce allocations from words in input file
     if (inFile.is_open())
    {
        while (count != 0)
        {  
            string s;
            getline(inFile, s);
            char const** word = (char const**)SegmentedMemoryAllocator::alloc(sizeof(char const*), FIRST_FIT);
            *word = s.c_str();
            //cout << *word << endl;
            words->push_back(word);
            
            if(count % INTERVAL == 0) {
                getrusage(RUSAGE_SELF, &rUsage);
                // Max set size used (in kilobytes)
                printf("alloc Memory usage = %ld ", rUsage.ru_maxrss);
                std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                auto current = now.time_since_epoch();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current);
                long currTime = duration.count();
                cout << "Time=" << currTime - subtract << endl;
            }
            count--;
        }  
    }
    cout << endl;
    // Deallocate
    for(int i = 0; i < MEDIUM; ++i) {

        SegmentedMemoryAllocator::dealloc(words->at(i));
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