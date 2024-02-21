# proc synchronization

### proc synchronization - how do they pass info to each other?
- make sure 2+ procs do not get in each other's way
  - e.g. 2 procs in an airline reservation system, each trying to grab last seat for different passenger
- ensure proper sequencing when dependencies present

### applicability to threads
- passing info between threads easy -  share same address space of parent proc
- other two aspects of proc synchronization applicable to threads
  - keep out of each other's business
  - proper sequencing

### producer-consumer problem
- implementation of ++/-- in machine language
  - ++: 
  ```asm
  register1 = counter
  register1 = register1 + 1
  counter = register1
  ```
  - --: 
  ```asm
  register1 = counter
  register1 = register1 - 1
  counter = register1
  ```
- these lower-level statements can be interleaved in any order

### race condition
- several procs access & manipulate data concurrently
- outcome of execution depends on particular order in which access takes place
- debugging programs w race conditions is painful. program runs fine most of the time, but once in a rare while something weird & unexpected happens
- example

### example of race condition
- when proc wants to print file, adds file to special spooler dir
- printer daemon periodically checks if there are files to be printed
  - if there are, print them
- for this example, spooler has lots of slots
- two vars
  - `in`: next free slot in dir
  - `out`: next file to be printed
- in jurisdictions where murphy's law holds
  - proc A reads `in`, stores value `7` in local var `next_free_slot`
  - context switch occurs
  - proc `B` also reads `in`, stores value `7` in local var `next_free_slot`, stores filename in slot `7`
  - proc `A` context switches again, stores name of file it wants to print in slot `7`
  - because of this, spooler dir internally consistent
  - but proc `B` will never receive any output & just loiters around

### kernel subject to several possible race conditions
- e.g. kernel maintains list of all open files
  - 2 procs open files simultaneously
  - separate updates to kernel list may result in race condition
- other kernel data structures
  - malloc
  - proc lists
  - interrupt handling

## critical section
- segment of code where procs change common vars

### critical section
- system of n procs
- each proc has a segment of code (critical section) where it changes common vars, updates table, &c.
- no 2 procs can execute their critical sections at the same time

### critical section problem
- design protocol that procs can use to cooperate
- each proc must request permission to enter its critical section (entry section)
- each proc must also have exit section - housekeeping that allows other procs to enter

### requirements for solution to critical section problem
1. mutual exclusion
  - only 1 proc can execute its critical section
  - when proc executes critical section, no other proc is allowed to execute its own critical section
2. progress
  - (condition 1) if no proc is executing its critical section
  - (condition 2) and some procs wish to enter their critical sections
  - decision on who gets to enter their critical section made by procs not executing their remainder section
  - selection cannot be indefinitely postponed
3. bounded wait
  - after proc has made a request to enter its critical section & before this request is granted
  - limit number of times other procs are allowed to enter their critical sections
- proc speed
  - each proc operates at nonzero speed
  - make no assumption about relative speed of the n procs

### approaches to handling critical sections in os
- nonpreemptive kernel
  - if proc runs in kernel mode, no preemption
  - free from race conditions on kernel data structures
- preemptive kernel
  - must ensure shared data is free from race conditions
  - difficult on smp (symmetric multiprocessor) architectures
    - 2 procs may run simultaneously on different procs

### why preempt?
- suitable for real-time
  - real-time proc may interrupt kernel proc
- more responsive
  - less risk that kernel mode proc will run arbitarily long

### peterson's solution
- software solution to critical section problem
  - restricted to 2 procs
- no guarantees on modern architectures
  - machine language instructions like `load` or `store` implemented differently
- good algorithmic description
  - shows how to address the 3 requirements
- components
  - restricted to 2 procs in this example but generalizable to n - P_i & P_j
  - share 2 data items
    - `int turn` - indicates whose turn it is to enter critical section
    - `boolean flag[2]` - whether proc is ready to enter critical section
- structure of proc P_i
```c
do {
  flag[0] = true;
  turn = 1;
  while (flag[0] && turn == 1) {;}
  // critical section
  flag[0] = false;
  // remainder section
} while (true);
```
- structure of proc P_j
```c
do {
  flag[1] = true;
  turn = 0;
  while (flag[0] && turn == 0) {;}
  // critical section
  flag[0] = false;
  // remainder section
} while (true);
```
- mutual exclusion
  - P_i enters critical section only if `!flag[j] || turn == i`
  - if both procs try to execute in critical section at the same time, `flag[0] == flag[1] == true`, but `turn` can only be 0 or 1
  - P_j enters critical section only if `flag[j] && turn == j` - will persist as long as P_j in critical section
- progress & bounded wait
  - P_i can be stuck only if `flag[j] && turn == j`
    - if P_j not ready, `flag[j] == false`, P_i can enter
    - once P_j exists, it resets `flag[j]` to false
  - if P_j sets `flag[j]` to `true`, must set `turn = i`
  - P_i will enter critical section after at most one entry by P_j (bounded wait)

## synchronization hardware

### solve critical section problem using locks
```c
do {
  // acquire lock
  // critical section
  // release lock
  // remainder section
} while (true);
```

### possible assists for solving critical section problem
- uniprocessor environment
  - prevent interrupts from occurring when shared var is being modified - no unexpected modifications
- multiprocessor environment
  - disabling interrupts time consuming - message passed to all processors
- special atomic hardware instructions
  - swap content of two words
  - modify word

### `Swap()`
```c
void Swap(boolean *a, boolean *b) {
  boolean temp = *a;
  *a = *b;
  *b = temp;
}
```
- shared var lock initialized to `false`
```c
do {
  key = true;
  while (key) {
    Swap(&lock, &key);
  }
  // critical section
  lock = false;
  // remainder section
} while (true);
```

### `TestAndSet()`
```c
boolean TestAndSet(boolean *target) {
  boolean rv = *target;
  *target = true;
  return rv;
}
```
- shared boolean var `lock` initialized to `false`
```c
do {
  while (TestAndSet(&lock)) {;} // to break out, `TestAndSet` should return false
  // critical section
  lock = false;
  // remainder section
} while (true);
```
- if 2 `TestAndSet()` executed simultaneously, they will be executed sequentially in some arbitrary order

