README
======

This package includes the following files.

- Driver.c [This is the driver program which will be used to invoke the MemoryManager.]
- MemoryManager.c [MemoryManager is implemented here.]
- MemoryManager.h [Header file declaring the function exposed from MemoryManager]
- README.txt [This file]

To compile:
    gcc *.c

To run:
    ./a.out <seed>

For example;
    ./a.out 1234
=====

I had fun with this assignment. Until now, I didn't know that data structures like arrays were stored on the heap,
although I did know that in order to create such a structure, the programmer must use `malloc`. During my testing,
I noticed that adding print statements greatly slowed down how quickly the program finished.

All in all, a nice, simple ease into this semester's programming assignments.

