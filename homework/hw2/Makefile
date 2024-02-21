# list of files
C_SRCS = coordinator.c checker.c 
C_OBJS = coordinator.o checker.o 

OBJS = ${C_OBJS}
EXE1 = coordinator
EXE2 = checker

# compiler, loader commands, & flags
GCC = gcc
GCC_FLAGS = -std=c11 -g -Wall -c -I.
LD_FLAGS = -std=c11 -g -Wall -I.

# compile .c files to .o files
.c.o:
	$(GCC) $(GCC_FLAGS) $<

# target is executable
all: coordinator checker
coordinator: coordinator.o
	$(GCC) $(LD_FLAGS) coordinator.o -o $(EXE1)
checker: checker.o
	$(GCC) $(LD_FLAGS) checker.o -o $(EXE2)

# clean up directory
clean:
	rm -rf *.o *- $(EXE1) $(EXE2)

# zip up everything for the submission
package:
	zip -r Habit-Blunk-HW2.zip coordinator.c checker.c Makefile README.txt
