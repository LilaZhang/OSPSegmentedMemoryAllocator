#include <iostream>
#include <string>
#include <vector>
#include "SegmentedMemoryAllocator.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#define ALLOC 'a'
#define DEALLOC 'd'
#define CHANGE 'c'
#define PRINT_ALLOCATED 'p'
#define PRINT_DEALLOCATED 'k'
#define EXIT 'x'

vector<int*> pointers;

void displayTitle();
void displayMenu();
void printPointers();
bool isANumber(string s);

int main() {
    
    bool exit = false;
    
    displayTitle();
    while(!exit) {
        displayMenu();
        cout << "-> ";
        char command;
        cin >> command;

        if(command == ALLOC) {
            size_t size = 0;
            string s;
            cout << "How many bytes to allocate? -> ";
            cin >> s;
            if(isANumber(s)) {
                int i = stoi(s);
                size = i;
                int* ptr = (int*)alloc(size);
                pointers.push_back(ptr);
                cout << endl;
                printPointers();
            }
            else {
                cout << "Please provide a valid number." << endl;
            }
        }
        else if(command == DEALLOC) {
            string s;
            unsigned int i = 0;
            int* addr;
            cout << "Index of memory address to deallocate -> ";
            cin >> s;
            if(isANumber(s)) {
                i = stoi(s);
                if(i < pointers.size() && i >= 0) {
                    addr = pointers.at(i);
                    dealloc(addr);
                    printPointers();
                }
                else {
                    cout << "Invalid index provided." << endl;
                }
            }
            else {
                cout << "Please provide a valid number." << endl;
            }
        }
        else if(command == CHANGE) {
            string s;
            int j = 0;
            cout << "Enter 1 for First, or 2 for Best, or 3 for Worst -> ";
            cin >> s;
            if(isANumber(s)) {
                j = stoi(s);
                if(j == 1 || j == 2 || j == 3) {
                    strategy s;
                    if(j == 1) {
                        s = FIRSTFIT;
                        cout << "Changed strategy to First-Fit." << endl;
                    }
                    else if(j == 2) {
                        s = BESTFIT;
                        cout << "Changed strategy to Best-Fit." << endl;
                    }
                    else if(j == 3) {
                        s = WORSTFIT;
                        cout << "Changed strategy to Worst-Fit." << endl;
                    }
                    setStrategy(s);
                }
                else {
                    cout << "Not a valid strategy." << endl;
                }
            }
            else {
                cout << "Please provide a valid number." << endl;
            }
        }
        else if(command == PRINT_ALLOCATED) {
            cout << "ALLOCATED LIST" << endl;
            printAllocatedList();
            cout << endl;
            printPointers();
        }
        else if(command == PRINT_DEALLOCATED) {
            cout << "DEALLOCATED LIST" << endl;
            printDeallocatedList();
            cout << endl;
            printPointers();
        }
        else if(command == EXIT) {
            exit = true;
            cout << "Goodbye." << endl;
        }
        else {
            cout << "Not a valid command." << endl;
        }
    }
}

void displayTitle() {
    cout << "\nSEGMENTED MEMORY ALLOCATOR S3718057\n==========================\n" << 
    "Note: Allocation strategy is by default set to First-fit.\nPointer type used in" << 
    " this demo class is int* only." << endl;
}

void displayMenu() {
    cout << "\nCOMMANDS\na     alloc(size_t size)\n" << 
    "d     dealloc(void* memoryAddress)\nc     Change Strategy (First=1, Best=2, Worst=3)" <<
    "\np     printAllocatedList()\n" << "k     printDeallocatedList()\nx     exit" << endl;
}

void printPointers() {
    cout << "POINTERS CREATED SO FAR" << endl;
    for(unsigned int i = 0; i < pointers.size(); ++i) {
        cout << "index " << i << ":   " << pointers.at(i) << endl;
    }
}

bool isANumber(string s) {
    bool isNumber = true;
    for(unsigned int i = 0; i < s.size() && isNumber; ++i) {
        if(!isdigit(s[i])) {
            isNumber = false;
        }
    }
    return isNumber;
}