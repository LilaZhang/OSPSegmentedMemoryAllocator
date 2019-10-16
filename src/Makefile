
allocator: SegmentedMemoryAllocator.o demo.o
	g++ -Wall -Werror -std=c++14 -g -O -o allocator SegmentedMemoryAllocator.o demo.o

demo.o: demo.cpp
	g++ -c demo.cpp

SegmentedMemoryAllocator.o: SegmentedMemoryAllocator.cpp SegmentedMemoryAllocator.h
	g++ -c SegmentedMemoryAllocator.cpp

clean:
	rm *.o allocator