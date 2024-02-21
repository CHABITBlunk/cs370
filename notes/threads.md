# threads (not the cpu kind)

## threads

### background
- exploited to make programs easier to write
  - split programs into separate tasks
- took off when guis became standard
  - user perceives better performance
    - programs didn't run faster (illusion)
    - dedicated thread to service input or display output
- growing trend to exploit available processors on a machine

### what threads are & why we use them
- miniproc or lightweight proc
- why would anyone want to have a kind of proc within a proc?
- in many apps, multiple activities are going on at once
  - some may block from time to time
- decompose app into multiple sequential threads
  - running quasi-parallel
- threads have ability to share address space & all of its data among themselves
- for several applications, procs with separate address spaces do work, but it's much more complex
- faster to create & destroy (10-100x)
- when num threads needed changes dynamically & rapidly, lightweight property is very useful

### performance argument
- when all threads are cpu bound all the time, additional threads would likely yield no performance gain
- but when there is substantial computing & i/o, having threads allows activities to overlap & possibly speeds up app

### example: word processor
- displays document on screen
- document formatted exactly as it will appear on printed page
- user deletes one sentence from p 1 of 800 page document
- now user wants to make change on p 600
  - word processor doesn't know what's the first line on p 600
  - word procesor has to reformat entire book up to page 600
  - threads could help
- suppose it's a 2-thread program
  - one for the user, one for formatting
  - as soon as sentence is deleted, interactive thread tells formatter thread to format book
- we could add a thread to auto-save every few minutes & handle disk backups without interfering with other 2 threads
- simpler with 3 threads
- 3 procs could work here, but that would need shared mem space, synchronizations, IPC, &c.

### apps are typically implemented as proc w multiple threads of control

## thread model

### proc model based on 2 independent concepts
- resource grouping
- execution

### proc can be thought of as a way to group related procs together
- address space containing program text & data
- other resources
  - open files, child procs, signal handlers, &c.

### proc has thread-of-execution
- usually shortened to thread
- thread has
  - program counter
  - registers: current working vars
  - stack: contains execution history - one frame for each procedure called but not returned from

### although a thread must execute in some proc
- proc & thread are different concepts
  - can be treated separately
- procs are used to group resources together
- threads are entities scheduled for execution on cpu

### threads & procs
- threads extend proc model by allowing multiple executions in same proc
- multiple threads in parallel on one proc is analogous to multiple procs running in parallel on one core

### different threads in a proc are not as independent as different procs
- all threads within a proc have same address space & share same global vars
- every thread can access every mem address within proc's address space
  - read
  - write
  - wipe out another thread's stack

### no protection between threads
- it is impossible
- it shouldn't be necessary

### contrasting items unique & shared across threads
- per proc items
  - address space
  - global vars
  - open files
  - child procs
  - pending alarms
  - signals & signal handlers
  - accounting info
- per thread items
  - program counter
  - registers
  - stack
  - state

### thread is a basic unit of cpu utilization
- thread id
- program counter
- register set
- stack
- state

### why each thread needs its own stack
- stack contains one frame for each procedure called but not returned from
- frame contains
  - local vars
  - procedure's return address

### thread states are similar to procs
- running
- blocked
- ready
- terminated

## benefits of multithreaded programming
- responsiveness
- resource sharing
- economy
- scalability

### rationale for threads
- proc creating is time consuming & resource intensive
- if new proc performs some tasks as existing proc, why incur this overhead?
- much more efficient to use multiple threads in proc

### threads have more inroads into os
- most os kernels are now multithreaded
  - perform specific tasks
  - interrupt handling
  - device mgmt
- solaris
  - multiple threads in kernel for interrupt handling
- linux
  - kernel thread manages system's free memory

### resource sharing
- programmer arranges sharing between procs
  - shared mem & message passing
- threads within proc share its resources
  - memory, code, & data
  - allows several different threads of activity within same proc

## user & kernel threads

### user threads
- threads that live in user space
- each proc has its own thread table
- invisible to kernel & have low overhead
- compete among themselves for resources allocated to their encapsulating proc
- scheduled by thread runtime system that is part of proc code
- programs link to special lib
  - each lib function enclosed by a jacket
  - jacket function calls thread runtime to do thread management before (& possibly after) calling jacketed lib function

### user thread libs - managing blocking calls
- replace potentially blocking calls w non-blocking ones
- if a call doesn't block, the runtime invokes it
- if the call may block
  1. place thread on list of waiting threads
  2. add call to list of actions to try later
  3. pick another thread to run
- all control is invisible to user & os

### user threads model disadvantages
- assumes that runtime will eventually regain control
  - thwarted by cpu bound threads or threads that rarely perform lib calls (runtime can't regain control to schedule other threads)
- programmer must avoid lockout situations
  - force cpu-bound thread to yield control
- can only share processor resources allocated to encapsulating proc - limits available parallelism

### kernel threads
- threads in kernel-space
- difference: kernel only has one thread table, rather than thread tables for individual procs (for our purposes)
- aware of kernel-level threads as schedulable entities
  - kernel maintains thread table to keep track of all threads in system
- compete systemwide for proc resources
  - can take advantage of multiple processors

### kernel threads - mgmt costs
- scheduling is almost as expensive as procs
  - synchronization & data sharing less expensive than procs
- more expensive to manage than user-level threads

### hybrid thread models
- write programs in terms of user-level threads
- specify number of schedulable entities associated w proc
  - mapping at runtime to achieve parallelism
- level of user control over mapping
  - implementation dependent

### many-to-one threading model
- several user-level threads mapped to one kernel thread
- thread mgmt done by thread library in user space
- when one thread makes a blocking system call, the entire proc blocks
- only 1 thread can access kernel at a time
  - multiple threads unable to run in parallel on multi-core/multiprocessor system
- e.g. solaris green threads, gnu portable threads

### one-to-one threading model
- one user thread mapped to one user thread
- more concurrency
  - another thread can continue to run when a thread invokes a blocking system call
- threads run in parallel on multiprocessors
- disadvantages
  - overhead for kernel thread creation
    - multiple user threads can degrade app performance
  - uses more kernel threads so uses more resources
- supported by
  - linux
  - windows nt/xp/2000
  - solaris 9+

### many-to-many threading model
- two-level is a variant of many-to-many
- multiplex many user-level threads on a smaller number of kernel threads
- number of kernel threads may be specific to a particular app or particular machine
- supported in irix, hp-us, & solaris prior to v9

| Column1    | many-to-one    | one-to-one    | many-to-many    |
|---------------- | --------------- | --------------- | --------------- |
| kernel concurrency    | no    | yes if many threads    | yes|
| during blocking system call    | proc blocks   | proc does not block other threads   | proc does not block   |
| kernel thread creation   | kernel thread already exists   | kernel thread creation overhead | kernel threads available   |
| caveat | use system calls (blocking) with care | don't create too many threads to not use too many resources | don't create too many threads to not use too many resources |

## thread libs

### thread libs
- provide api for creating & managing threads
- include functions for:
  1. thread creation & destruction
  2. enforcement of mutual exclusion
  3. conditional waiting
- runtime system to manage threads
  - users not aware of this

### user-level thread libs
- uses kernel support for mapping user threads to kernel threads
- lib code & data structures reside in user space
- invoking lib function does not result in a system call
  - local function call in user space

### kernel-level thread libs
- lib code & data structures reside in kernel space
- invoking lib function typically results in system call
- typical approach: user-level thread libs access kernel-level thread lib api to map user threads to kernel threads, but this is hidden in user thread lib runtime implementation

| Column1    | user-level thread lib    | kernel-level thread lib|
|---------------- | --------------- | --------------- |
| lib code & data structures    | reside in user space    | reside in kernel space |
| thread creation requires system call    | yes    | no    |
| os/kernel support| no   | yes   |

### dominant thread libs
- posix pthreads
  - extends posix standard (ieee 1003.1c)
  - provided as user-level or kernel-level lib
  - solaris, macos, linux, &c.
- win32 thread lib
  - kernel-level only
- java threading api
  - implemented using thread lib on host system
    - on windows: threads use win32 api
    - unix/linux: use pthreads

### pthreads - spec for thread behavior, not implementation
- return 0 if successful
- ***do not set errno***
- `pthread_cancel` - terminate another thread
  - takes thread id of target
  - depends on type & state of thread
  - type allows thread to control when to exit
    - `PTHREAD_CANCEL_ASYNCHRONOUS` - any time
    - `PTHREAD_CANCEL_DEFERRED` - only at specified cancellation points
  - state
    - `pthread_cancelstate` to change state
      - `PTHREAD_CANCEL_ENABLE`
      - `PTHREAD_CANCEL_DISABLE` - cancellation requests are held pending
- `pthread_create` - create thread
  - automatically makes thread runnable
  - 3 params
    - id of newly created thread
    - attributes for thread (stack size, scheduling info, &c.)
    - pointer to function that thread calls when begins execution
- `pthread_detach` - set thread to release resources
  - when thread exits it doesn't release its resources unless it is detached
  - sets internal opts to specify that storage for thread can be reclaimed when it exits
  - 1 param - thread id of thread to detach
- `pthread_exit` - exit thread without exiting proc
  - if function running calls `exit`, all thread terminate
  - call `pthread_exit` causes only calling thread to terminate
  - threads can force other threads to return through cancellation mechanism
- `pthread_kill` - send signal to thread
- `pthread_join` - wait for thread
  - threads that are not detached are joinable
  - undetached threads don't release resources until another thread calls `pthread_join` for them or proc exits
  - params
    - id of thread to wait for
    - suspends calling thread until target terminates
    - similar to `waitpid` at proc level
    - `pthread_join(pthread_self())` - deadlock
- `pthread_self` - find out own thread id

### pthreads example
- use a thread to perform a summation of a non-negative integer
- example
```c
#include <pthread.h>
#include <stdio.h>

int sum; // this data is shared by thread(s)

void *runner(void *param); // the function to compute the sum

int main(int argc, char *argv[]) {
  pthread_t tid; pthread_attr_t attr;
  // get default attributes
  pthread_attr_init(&attr);

  // create thread
  pthread_create(&tid, &attr, runner, argv[1]);

  // now wait for thread to exit
  pthread_join(&tid, NULL);

  printf("sum = %d\n", sum);
}

// thread will begin control in this function
void *runner(void *param) {
  int i, upper = atoi(param);
  sum = 0;

  if (upper > 0) {
    for (i = 1; i <= upper; i++) sum += i;
  }
  pthread_exit(0);
}
```

### java threads
- designed from ground up to support concurrent programming
  - basic concurrency support in language & class libs
- java 1.5+ has powerful concurrency apis

### jvms harness the thread models of host os
- windows xp has one-to-one
  - so a thread maps to a kernel thread
- tru64 unix has many-to-many
  - java threads mapped accordingly
- solaris
  - initially used green threads - many-to-one
  - v9+: many-to-one model

### creating threads in java
- create new class derived from `Thread` & override `run()` method
- more commonly used, `Runnable` interface
  - has 1 method `run()`
  - create new `Thread` class by passing `Runnable` object to its constructor
- `Executor` interface (`java.util.concurrent`)
  - 1 method `execute()`
- example:
```java
class Sum {
  private int sum;
  public int get() {
    return sum;
  }
  public void set(int sum) {
    this.sum = sum;
  }
}

class Summation implements Runnable {
  private int upper;
  private Sum sumValue;

  public Summation(int upper, Sum sumValue) {
    this.upper = upper;
    this.sumValue = sumValue;
  }

  public void run() {
    int sum = 0;
    for (int i = 0; i <= upper; i++) sum += i;
    sumValue.set(sum);
  }
}

public class Driver {
  public static void main(String[] args) {
    Sum sumObject = new Sum();
    int upper = Integer.parseInt(args[0]);

    Thread worker = new Thread(new Summation(upper, sumObject));
    worker.start();
    try {
      worker.join();
    } catch (InterruptedException ie) {
      ie.printStackTrace();
    }
    System.out.println("The sum of " + upper + " is " + sumObject.get());
  }
}
```

### win32 threads
- `CreateThread` - pass security info, size of stack, flag (start in suspended state?)
- `WaitForSingleObject`
- `CloseHandle`

## thread pools

### thread pools
- create a number of threads at startup
- place them into a pool
- threads will sit & wait for work
- advantages:
  - servicing requests faster w existing threads
  - limits total number of threads
- when work needs to be performed
  - awaken a thread from pool
  - assign it work
  - once complete, thread returns to pool
