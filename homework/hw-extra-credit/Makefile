# list of files
C_SRCS = profiler.c
C_OBJS = profiler.o
EXE = profiler

# compiler, loader commands, & flags
GCC = gcc
GCC_FLAGS = -std=c11 -g -Wall -c -I.
LD_FLAGS = -std=c11 -g -Wall -I.

all: profiler
profiler: profiler.o
	$(GCC) $(LD_FLAGS) profiler.o -o $(EXE) 

clean:
	rm -rf *.o *- $(EXE) *.tar

package:
	tar cvf Blunk_Habit_ExtraCredit.tar $(C_SRCS) README.txt Makefile
