# procs

### proc
- unit of work in most systems
- just an instance of an executing program
- each proc has its own virtual CPU
- procs not affected by multiprogramming
- at any instant only one proc executes

### when a proc is interrupted
- computer must appreciate urgency

### how a program becomes a proc
- when program is run, os copies program image into main memory
- allocation of memory not enough to make program into proOSc
- must have proc ID
- os tracks IDs & proc states to orchestrate system resources

### program in memory
- program image appears to occupy contiguous blocks of memory
- os maps programs into non-contiguous blocks
- mapping divides program into equal-sized pieces called pages
- os loads pages into memory
- when procor references memory on page, os looks up page in table & loads it into memory

### advantages of mapping
- allows large logical address space for stack & heap - no physical memory used unless actually needed

### proc control block (PCB)
- how procs are represented
- contains
    - state
    - number
    - counter
    - registers
    - memory limits
    - list of open files
- repository of proc information

### scheduling queues
- job queue: contains all procs
    - newly created procs enter here first
- ready queue
    - procs remaining in main memory
    - ready & waiting to execute
    - typically linked list
- device queue
    - procs waiting for particular I/O device

### proc migration
- long-term scheduler: batch systems
    - executes much less frequently
    - can take more time to decide what to select
- short-term scheduler
    - frequently procs for CPU
    - selected proc executes every few ms
    - typically, execute every 10-100ms
- unix & Windows often have no long-term scheduler
    - every new proc goes on short-term
    - stability depends on physical limitations & self-adjusting nature of users

## interrupts & contexts

### factors that impact speed of context switch
- memory speed
- number of registers to copy
- special instructions for loading/storing registers
- memory management: preservation of address space

### how to create subprocs
- use `fork()`
    - when `fork()` fails
        - no child is created
        - `fork()` returns -1

### replace a proc's memory space w a new program
- use `exec()` after `fork()` in one of two procs
- things `exec()` does
    - destroys memory image of program containing call
    - replaces invoking proc's memory space w new program
    - allows procs to go separate ways
- traditionally
    - child executes new program
    - parent executes original code

### using shell to launch programs is done in two steps
- user types `ls` in shell
    1. shell `fork()`s off new proc
    2. child executes `ls`
- it's done this way because it allows child to manipulate file descriptors
    - after `fork()` but before `exec()`
- accomplish redirection of stdin, stdout, & stderr

### parents can remove themselves from ready queue & wait for child's termination
- done using `wait()` call
- when child proc completes, parent proc resumes

### `wait()`/`waitpid()` allows caller to suspend execution until child's status is available
- proc status availability
    - most commonly after termination
    - also available if proc is stopped
- `waitpid(pid, *stat_loc, options)`
    - `pid == -1`: any child
    - `pid > 0`: specific child
    - `pid == 0`: any child in same proc group
    - `pid < -1`: any child in proc group `abs(pid)`

### windows proc creation
- `CreateProcess` handles proc creation & loading in new program
- parent & child's address spaces are different from the start
- takes up to 10 params
    - program to be executed
    - cli params
    - security attrs
    - control bits for inheritance
    - priority info
    - window to be created?

### windows proc mgmt
- win32 has about 100 other functions for managing & synchronizing procs

## proc groups
- collection of procs
- each proc has a proc group id
- proc group leader = proc w pid == pgid
- `kill` treats negative pid as pgid
    - sends signal to all constituent procs
- pgid: when child is created w `fork()`
    - inherits parent's pgid
    - parent can change child pgid using `setpgid()`
    - child can give itself new pgid
        - set pgid = its pid
- can contain parent, sibling, & child procs
- proc can only send signals to members of its proc group

### windows has no concept of proc hierarchy
- only hint of hierarchy: when proc is created, parent given special token (handle) used to control child
- parent is free to pass handle to some other proc
    - Invalidates hierarchy

## proc terminations
- normal exit (voluntary)
    - finished compiling
- error exit (involuntary)
    - compiling a file that doesn't exist
- fatal error (involuntary)
    - program bug
- killed by other proc (involuntary)
    - execute system call telling os to kill some other proc
    - killer must be authorized to kill the killee
    - unix: `kill()` win32: `TerminateProcess()`

### proc terminations: normal or abnormal
- os deallocates proc resources
    - cancel pending timers & signals
    - release virtual memory resources & locks
    - close any open files
- updates stats
    - proc status & resource usage
- notifies parent in response to `wait()`
- unix: on termination proc does not fully release resources until parent calls `wait()` for it
    - when parent not waiting when child terminates, proc becomes zombie
    - zombie is inactive proc, i.e. still has entry in proc table, but is already dead, so cannot be killed easily
    - zombies come from error in programming: not properly waiting on all children created, changing parent while children still active, &c.
- when proc terminates, orphaned children are adopted by init
- init properties
    - pid 1
    - periodically executes `wait()` for children
    - orphans adopted by init
    - zombies also adopted & killed after periodic wait
- normal termination of proc
    - return from `main()`
    - implicit return from `main()`, function falls off end
    - call to `exit()`, `_Exit()`, or `_exit()`

### C `exit()` function
```c
#include <stdlib.h>
int atexit(void (*func) ())
```
- call user-defined exit handlers registered by `atexit()`
    - invocation is in reverse order of registration
    - execute function pointed by func when proc terminates
- flushes any open streams that have unwritten buffer data
- closes all open streams
- remove all temp files created by `tmpfile()`
- `_Exit()` & `_exit()` do not call user-defined exit handlers because POSIX doesn't specify what happens
- all functions (`exit()`, `_Exit()`, & `_exit()`) take parameter status
    - indicates termination status of proc
    - 0 is successful termination
    - non-zero values: programmer-defined errors

### abnormal termination
- call `abort()` - proc signal that causes termination
    - can be generated by external event: keyboard `<C-c>`
    - internal errors: access illegal mem location
- consequences
    - core dump
    - user-installed exit handler not called

## protection & security
- Control access to system resources - improve reliability
- Defend against misuse by unauthorized or incompetent users
- Examples
    - Ensure proc executes in its own space
    - Force procs to relinquish cpu control
    - Device-control registers accessible only to os

### buffer overflow
- When program copies data into variable for which it has not allocated enough space

### automatic variables (local variables)
- allocated/deallocated automatically when program flow enters/leaves var scope
- allocated on program stack
- stack grows from high-memory to low-memory

### problems w buffer overflow
- function will try to return to address space outside program
    - segfault/core dump
    - programs may lose unsaved data
    - in os, such a function can cause it to crash

### 1988-11-12
- 2 bugs in berkeley unix
- worm
- brought down sun & vax systems from internet within a few hours
- two programs compiled & executed on system under attack
    - bootstrap (99 sloc of c, l1.c)
    - worm proper

