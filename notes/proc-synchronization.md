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
  - restricted to 2 procs in this example but generalizable to n - `P_i` & `P_j`
  - share 2 data items
    - `int turn` - indicates whose turn it is to enter critical section
    - `boolean flag[2]` - whether proc is ready to enter critical section
- structure of proc `P_i`
```c
do {
  flag[0] = TRUE;
  turn = 1;
  while (flag[0] && turn == 1) {;}
  // critical section
  flag[0] = FALSE;
  // remainder section
} while (TRUE);
```
- structure of proc `P_j`
```c
do {
  flag[1] = TRUE;
  turn = 0;
  while (flag[0] && turn == 0) {;}
  // critical section
  flag[0] = FALSE;
  // remainder section
} while (TRUE);
```
- mutual exclusion
  - `P_i` enters critical section only if `!flag[j] || turn == i`
  - if both procs try to execute in critical section at the same time, `flag[0] == flag[1] == TRUE`, but `turn` can only be 0 or 1
  - `P_j` enters critical section only if `flag[j] && turn == j` - will persist as long as `P_j` in critical section
- progress & bounded wait
  - `P_i` can be stuck only if `flag[j] && turn == j`
    - if `P_j` not ready, `flag[j] == FALSE`, `P_i` can enter
    - once `P_j` exists, it resets `flag[j]` to FALSE
  - if `P_j` sets `flag[j]` to `TRUE`, must set `turn = i`
  - `P_i` will enter critical section after at most one entry by `P_j` (bounded wait)

## synchronization hardware

### solve critical section problem using locks
```c
do {
  // acquire lock
  // critical section
  // release lock
  // remainder section
} while (TRUE);
```

### possible assists for solving critical section problem
- uniprocessor environment
  - prevent interrupts from occurring when shared var is being modified - no unexpected modifications
- multiprocessor environment
  - disabling interrupts time consuming - message passed to all processors
- special atomic hardware instructions
  - swap content of two words - `Swap()`
  - modify word - `TestAndSet()`

### `Swap()`
```c
void Swap(boolean *a, boolean *b) {
  boolean temp = *a;
  *a = *b;
  *b = temp;
}
```
- shared var lock initialized to `FALSE`
```c
do {
  key = TRUE;
  while (key) {
    Swap(&lock, &key);
  }
  // critical section
  lock = FALSE;
  // remainder section
} while (TRUE);
```

### `TestAndSet()`
```c
boolean TestAndSet(boolean *target) {
  boolean rv = *target;
  *target = TRUE;
  return rv;
}
```
- shared boolean var `lock` initialized to `FALSE`
```c
do {
  while (TestAndSet(&lock)) {;} // to break out, `TestAndSet` should return FALSE
  // critical section
  lock = FALSE;
  // remainder section
} while (TRUE);
```
- if 2 `TestAndSet()` executed simultaneously, they will be executed sequentially in some arbitrary order

### using TestAndSet to satisfy critical section requirements
- n procs
- data structures = FALSE
  - `boolean waiting[n]`
  - `boolean lock`
- entry section for proc i
  ```c
  waiting[i] = TRUE;
  key = TRUE;
  while (waiting[i] && key) {
    key = TestAndSet(lock);
  }
  waiting[i] = FALSE;
  ```
  - 1st proc to execute TestAndSet will find `key == FALSE`
    - enter critical section
      - everyone else must wait
  - exit section for proc i
    ```c
    j = (i + 1) % n;
    while ((j != i) && !waiting[j]) {
      j = (j + 1) % n
    }
    if (j == 1) {
      lock = FALSE;
    } else {
      waiting[j] = FALSE;
    }
    ```

### mutual exclusion
- `waiting[i]` can become `FALSE` **only if** another proc leaves its critical section
  - only one `waiting[i] = FALSE`

### progress
- proc exiting critical section does `lock = FALSE` or `waiting[j] = FALSE`
- allows waiting proc to proceed

### bounded waiting requirement
```c
j = (i + 1) % n;
while ((j != 1) && !waiting[j]) {
  j = (i + 1) % n;
}
```
- scans waiting[] in cyclic ordering (`i + 1, i + 2, ..., n, 0, ..., i - 1`)
- any waiting proc trying to enter critical section will do so in `n - 1` turns

## semaphores
- semaphore `S` is an int var
- once initialized, accessed through atomic operations
  - `wait()`
  - `signal()`

### mods to int value of semaphore execute indivisibly
```c
wait(S) {
  while (S <= 0) {
    ;
  }
  S--;
}
```
```c
signal(S) {
  S++;
}
```

### types of semaphores
- binary semaphores (mutex locks)
  - value of `S` can be `0` or `1`
- counting semaphores
  - value of `S` can range over unrestricted domain

### using binary semaphore to deal with critical section problem
- `mutex = 1`:
  ```c
  do {
    wait(mutex);
    // critical section
    signal(mutex);
    // remainder section
  } while (TRUE);
  ```
- suppose we require `S2` to execute only after `S1` has executed
  - `synch = 0`, wait for it to be > 0 and run S2, then start S1

### counting semaphore
- controls access to finite set of resource instances
- initialized to number of resources available
- resource usage
  - `wait()`: to use a resource
  - `signal()`: to release a resource
- when all resources are being used, `S == 0`
  - block until `S > 0` to use resource

### problems with basic semaphore implementation
- case 1: if there is a proc in critical section
- case 2: if another proc tries to enter its critical section
- must loop continuously in entry code
  - busy waiting - some other procs could have used this more productively
- sometimes these locks are called spinlocks
  - one advantage: no context switch needed when proc must wait on lock

### overcoming need to busy wait
- during `wait` if `S == 0`
  - instead of busy waiting, proc blocks itself
  - place proc in waiting queue for `S`
  - proc state switched to waiting
  - cpu scheduler picks another proc to execute
- restart proc when another proc does `signal()`
  - restarted using `wakeup()`
  - change state from waiting to ready

### define semaphore
```c
typedef struct {
  int value;
  struct process *list; // list of procs
} semaphore;
```

### `wait()` operation to eliminate busy waiting
```c
wait(semaphore *S) {
  S->value--;
  if (S->value < 0) {
    // add proc to S->sleeping_list;
    block();
  }
}
```

### `signal()` operation to eliminate busy waiting
```c
signal(semaphore *S) {
  S->value++;
  if (S->value <= 0) {
    // remove proc Pfrom S->sleeping_list;
    wakeup(P);
  }
}
```

### deadlocks & starvation
- proc p0; proc p1
  - `wait(S)`; `wait(Q)`
  - `wait(Q)`; `wait(S)`
  - `signal(S)`; `signal(Q)`
  - `signal(Q)`; `signal(S)`
- say p0 executes `wait(S)` and then P1 executes `wait(Q)`
  - p0 must wait till p1 executes `signal(Q)`
  - p1 must wait till p0 executes `signal(S)`

### semaphores & atomic operations
- once a semaphore action has started, no other proc can access semaphore until proc done or blocked
- atomic operations - group of related operations performed without interruptions or not at all

## priority inversion
- procs l, m, h (priority l < m < h)
- h requires resource `r` accessed by l
  - typically, h will wait for l to finish with `r`
- m becomes runnable and preempts l
  - proc m with lower priority affects how long proc h has to wait for l to release `r`

### priority inheritance protocol
- proc accessing resource needed by higher priority proc inherits higher priority until finished with resource
  - once done, reverts to lower priority

## classic problems of synchronization

### bounded buffer problem
- binary semaphore (mutex)
  - provides mutual exclusion for accesses to buffer pool
  - initialized to 1
- counting semaphores
  - empty: number of empty slots available to produce - initialized to n
  - full: number of filled slots available to consume - initialized to 0

### some other things to bear in mind
- producer & consumer must be ready before they attempt to enter critical section
- producer readiness - when a slot is available to add produced item
  - `wait(empty)`: `empty` initialized to n
- consumer readiness - when producer has added new item to buffer
  - `wait(full)`: `full` initialized to 0

### producer
```c
do {
  // produce item nextp
  wait(empty);
  wait(mutex);
  // add nextp to buffer
  signal(mutex);
  signal(full);
  // remainder section
} while (TRUE);
```

### consumer
```c
do {
  wait(full);
  wait(mutex);
  // remove nextc from buffer
  signal(mutex);
  signal(empty);
  // consume nextc
} while (TRUE);
```

## readers-writers problem
- database shared among several concurrent procs
- 2 types of procs: readers & writers

### potential for adverse effects
- if 2 readers access shared data simultaneously, no problem
- if a writer & a some other reader (or writer) access shared data simultaneously, problem

### writers must have exclusive access to shared database before writing
- 1st readers-writers problem
  - no reader should wait for other readers to finish simply because a writer is waiting
    - writers may starve
- 2nd readers-writers problem
  - if a writer is ready it writes asap
    - readers may starve

### solution to 1st readers-writers problem
- `int readcount` - tracks how many readers are reading object
- semaphore `mutex` - ensure mutex when `readcount` accessed
- semaphore `wrt` - mutex for writers
  - 1st (last) reader that enters (exits) critical section
    - not used by readers when other readers are in their critical section
- writer
  ```c
  do {
    wait(wrt);
    // writing performed
    signal(wrt);
  } while TRUE;
  ```
- reader
  ```c
  do {
    wait(mutex);
    readcount++;
    if (readcount == 1) {
      wait(wrt);
    }
    signal(mutex);
    // reading performed
    wait(mutex);
    readcount--;
    if (readcount == 0) {
      signal(wrt);
    }
    signal(mutex);
  } while (TRUE);
  ```

## dining philosophers problem
- 5 philosophers, 5 plates, 5 chopsticks in between (1 between each 2)

### problem
- philosopher tries to pick up 2 closest (lr) chopsticks
- pick up only 1 chopstick at a time
  - cannot pick up a chopstick in use
- eat only when have 2 chopsticks
- when done, put down 2 chopsticks
- represents allocation of several resources among several procs
- can be done so that it is deadlock free & starvation free

### simple solution
- each chopstick is a semaphore
  - grab by executing `wait()`
  - release by executing `signal()`
- shared data
  - `semaphore chopstick[5]`
  - all elements = 1

### what if all philosophers get hungry & grab same (l/r) chopstick?
```c
do {
  wait(chopstick[i]);
  wait(chopstick[(i + 1) % 5]);
  // eat
  signal(chopstick[i]);
  signal(chopstick[(i + 1) % 5]);
  // think
} while (TRUE);
```

## monitors

### overview of semaphore solution
- procs share semaphore `mutex`
  - initialized to 1
- each proc must execute `wait` before entering critical section & `signal` after exiting critical section

### incorrect use of semaphores can lead to timing errors
- hard to detect
  - reveal themselves only during specific execution sequences
- if correct sequence not observed, 2 procs may be in critical section simultaneously
- problems even if only one proc is not well behaved
- interchange order of `wait` & `signal`
  ```c
  do {
    signal(mutex);
    // critical section - several procs simultaneously active in critical section
    wait(mutex);
    // remainder section
  } while (TRUE);
  ```
- replace `signal` with `wait`
  ```c
  do {
    wait(mutex);
    // critical section
    wait(mutex); // deadlock
    // remainder section
  }
  ```
- omit signal and/or wait
  ```c
  do {
    wait(mutex); // omission -> mutex violated
    // critical section
    signal(mutex); // omission -> deadlock
    // remainder section
  } while (TRUE);
  ```

### improper usage of semaphores -> problems
- need for a higher-level synchronization construct: monitor
- abstract data types
  - encapsulates private data w public methods to operate on them

### monitor is abstract data type
- mutex provided within monitor
- contains
  - declaration of vars defining instance's state
  - functions that operate on these vars

### monitor construct ensures only one proc at a time is active in monitor
```c
monitor monitor_name {
  // shared var declarations
  function F1(..) {...}
  function F2(..) {...}
  function Fn(..) {...}
  // initialization code(..) {...}
}
```

### basic monitor scheme not sufficiently powerful
- provides easy way to achieve mutex
- we also need a way for procs to block when they cannot proceed
- solution: condition construct
  - `condition x, y;`
  - operations on a condition variable
    - `x.wait` - proc invoking this is suspended until `signal` call
    - `x.signal` - resumes exactly 1 suspended proc
      - if no proc waiting, no effect on state of `x`

### semantics of wait & signal
- `x.signal()` invoked by proc `P`
- `Q` is suspended proc waiting on `x`
- signal & wait: `P` waits for `Q` to leave monitor
- signal & continue: `Q` waits until `P` leaves monitor
- PASCAL: when thread `P` calls signal
  - `P` leaves immediately
  - `Q` immediately resumed

### difference between `signal()` in semaphores & monitors
- monitors (condition vars): not persistent
  - if a signal is performed & no waiting threads, signal ignored
  - during subsequent `wait` operations, thread blocks
- semaphores
  - signal increments semaphore value even if there are no waiting threads
    - future `wait`s would immedidately succeed

### dining philosophers using monitors
```c
enum {THINKING, HUNGRY, EATING} state[5];
if (state[(i + 4) % 5] != EATING && state[(i + 1) % 5] != EATING) {
  state[i] = EATING
} 
condition self[5] // delay self when HUNGRY but unable to get chopsticks
```

### sequence of actions
- before eating, must invoke `pickup()`
  - may result in suspension of philosopher proc
  - after completion of operation, philosopher may eat

### `pickup()` & `putdown()` operations
```c
pickup(int i) {
  state[i] = HUNGRY;
  test(i);
  if (state[i] != EATING) {
    self[i].wait(); // wait if unable to find chopstick
  }
}
```
```c
putdown(int i) {
  state[i] = THINKING;
  test((i + 4) % 5);
  test((i + 1) % 5);
}
```

### `test()` - see if philosopher can eat
```c
test(int i) {
  if (state[(i + 4) % 5] != EATING && // if right not eating...
      state[i] == HUNGRY && // and hungry...
      state[(i + 1) % 5] != EATING) { // and left not eating...
    state[i] = EATING;
    self[i].signal(); // signal a proc that was suspended while trying to eat
  }
}
```

### possibility of starvation
- philosopher `i` can starve if eating periods of philosophers on left & right overlap
- possible solution
  - introduce new state: `STARVING`
  - chopsticks can be picked up if no neighbor is waiting
    - effectively wait for neighbor's neighbor to stop eating
    - reduces concurrency

## implementing monitor using semaphores

### implementing monitor using semaphores
- for each monitor, set semaphore `mutex = 1`
- proc must execute
  - `wait(mutex)`: before entering monitor
  - `signal(mutex)`: after entering monitor

### semantics of signaling proc
- signaling proc must wait until resumed proc leaves or waits
  - additional semaphore `next` introduced
- so signaling proc needs to suspend itself
  - `next = 0`
    - signaling proc must use `next` to suspend themselves
  - `int next_count`
    - count num procs suspended on `next`

### implementing function `f` in monitor
```c
wait(mutex);
// implementation for f
if (next_count > 0) {
  signal(next);
} else {
  signal(mutex);
}
```

### implementing condition vars
```c
// x.wait() operation
// for each condition x we have, semaphore x_sem = 0, int x_count = 0
x_count++;
if (next_count > 0) {
  signal(next);
} else {
  signal(mutex);
}
wait(x_sem);
x_count--;

// x.signal() operation
if (x_count > 0) {
  next_count++;
  signal(x_sem);
  wait(next);
  next_count--;
}
```

### resuming procs within monitor
- case 1: several procs suspended on condition `x`
- case 2: `x.signal()` executed by some proc
- which suspended proc should be resumed next?
  - simple solution: fcfs ordering (proc waiting longest resumed first)

### proc resumption: conditional `wait`
- `x.wait(c)`
- `c` is int expression, evaluated when `wait()` executed
- value of `c` is priority number
  - stored with name of suspended proc
- when `x.signal()` executed
  - proc with smallest priority number resumed next

### monitor to allocate single resource
```c
monitor ResourceAllocator {
  boolean busy;
  condition x;

  void acquire(int time) {
    if (busy) {
      x.wait(time);
    }
    busy = TRUE;
  }

  void release() {
    busy = FALSE;
    x.signal();
  }
  initialization() { busy = FALSE; }
}
```

### example of conditional waits
```c
R.acquire(t); // specify max time resource will be used, monitor allocates resource based on shortest duration
// access resource
R.release();
```
- monitor cannot guarantee that access sequence will be observed

### avoiding time dependent errors & ensuring that scheduling algorithm not defeated
- user procs must make their calls on monitor in correct sequence
- ensure that uncooperative procs do not ignore mutex gateway - should not access this resource directly!

