README
======

This package includes the following files.
- checker.c [Child process of coordinator.c, responsible for computing if the two numbers passed to it from coordinator.c have a common factor]
- coordinator.c [File that takes in 5 arguments from stdin and creates child tasks to see if the first number is a factor of each subsequent number]
- Makefile [Tells Make how to compile checker.c and coordinator.c to create two C objects of each and two binaries of each]
- README.txt [This file]

To compile:
  make all

To run:
  ./coordinator <divisor> <dividend 1> <dividend 2> <dividend 3> <dividend 4>

For example:
    ./coordinator 7 32 49 846 22344
=====

This assignment was fun to tinker with. I didn't know how much more closely you have to pay attention to when you call things in C, especially with regards to getpid().
Interestingly enough, execlp() doesn't work on my laptop, so I used execl() instead. Will test again on CSU machines when I get a chance tomorrow.
