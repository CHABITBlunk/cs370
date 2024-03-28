# atomic transactions

## synchronization examples

### synchronization in solaris
- condition vars
- semaphores
- adaptive mutexes
- reader-writer locks
- turnstiles

### solaris adaptive mutex
- starts as standard semaphore implemented as spinlock
- on smp systems if data is locked & cannot use?
  - if lock held by thread on another cpu, spin waiting for lock to be available
  - if thread holding lock is not running, block until awakened by release of lock
- single processor system
  - thread sleeps instead of spinning when lock is encountered

### adaptive mutex only used for short code segments
- less than a few hundred instructions
  - spinlocks inefficient for code segments larger than that
- cheaper to put a thread to sleep & awaken it
  - busy waiting in spinlock is expensive
- longer code segments?
  - condition vars & semaphores used

### reader-writer locks
- used to protect data accessed frequently - usually accessed in read-only manner
- multiple threads can read data concurrently unlike semaphores that serialize access to data
- relatively expensive to implement - used only on long sections of code

### solaris: turnstiles
- queue structure containing threads blocked on lock
- used to order threads waiting to acquire adaptive mutex or reader-writer lock
- each kernel thread has its own turnstile as opposed to every synchronized object
  - thread can be blocked only on one object at a time
- turnstile for 1st thread to block on synchronized object becomes turnstile for object itself
  - subsequent threads blocking on lock added to this turnstile
- when this first thread releases its lock, it gains a new turnstile from list of free turnstiles maintained by kernel

### turnstiles organized according to priority inheritance control
- if thread holding a lock on which a higher priority thread is blocked?
  - temporarily inherit priority of higher priority thread
  - revert back to original priority after releasing lock

### linux: prior to 2.6, linux was nonpreemptive
- provides spinlocks & semaphores
| single processor   | multiprocessor    |
| disable kernel preemption   | acquire spinlock   |
| enable kernel preemption   | release spinlock   |

### kernel is not preemptible if kernel-mode task is holding a lock
- each task has a thread-info structure
  - counter `preempt_count` indicates number of locks being held by a task
  - `preempt_count` incremented when lock acquired
    - decremented when lock released
  - if `preempt_count > 0`, not safe to preempt
    - ok otherwise, if no `preempt_disable()` calls pending

### linux: other mechanisms
- atomic ints `atomic_t`
  - all math operations using atomic ints performed without interruption
- mutex locks
  - `mutex_lock()`: prior to entering critical section
  - `mutex_unlock()`: after exiting critical section
  - if lock unavailable, task calling `mutex_lock()` put to sleep
    - awakened when another task calls `mutex_unlock()`

## atomic transactions

### atomic transactions
- mutex of critical sections ensures their atomic execution as one uninterruptible unit
- also important to ensure that critical section forms single logical unit of work
  - either work is performed in its entirety or not at all
  - e.g. transfer of funds - credit one account & debit the other

### transaction
- collection of operations performing single logical function
- preservation of atomicity despite possibility of failures

### disk i/o transaction that accesses/updates data items on disk
- simply a sequence of read/write operations terminated by commit or abort
- commit: successful transaction termination
- abort: unsuccessful transaction termination due to logical error or system failure

### transaction rollbacks
- aborted transaction may have modified data
- state of accessed data must be restored to what it was before transaction began executing

### log-based recovery to ensure atomicity: rely on stable storage
- record info describing all modifications made by transaction to various accessed data
- each log record describes a single write
  - transaction name
  - data item name
  - old value
  - new value
- other log records exist to record significant events
  - start of transaction, commit, abort, &c

### actual update cannot take place prior to logging
- prior to `write(x)` operation
  - log records for `x` should be written to stable storage
- 2 physical writes for every logical write - more storage needed
- functionality worth price: data that is extremely important for fast failure recovery

### populating entries in log
- before transaction `T_i` starts execution, record `<T_i starts>` to log
- any write by `T_i` preceded by writing to log
- when `T_i` commits, record `<T_i commits>` to log

### system can handle any failure without loss of information: log
- `undo(T_i)` - restore value of all data updated by `T_i` to old values
- `redo(T_i)` - set value of all data updated by `T_i` to new values
- `undo(T_i)` & `redo(T_i)`
  - idempotent
  - multiple executions have same result as 1 execution

### if system failure occurs restore state by consulting log
- determine which transactions need to be undone & which need to be redone
- `T_i` undone if log contains `<T_i starts>` but no `<T_i commits>` record
- `T_i` redone if log contains `<T_i starts>` & `<T_i commits>`

## checkpointing

### rationale for checkpointing
- when failure occurs we consult log for undoing or redoing
- but if done naively, we need to search entire log
  - time consuming
  - recovery takes longer, though no harm done by redoing (idempotency)

### in addition to write-ahead logging, periodically perform checkpoints
- output following to stable storage
  - all log records residing in main memory
  - all modified data residing in main memory
  - a log record `<checkpoint>`
- `<checkpoint>` allows system to streamline recovery procedure

### implications of checkpoint record
- `T_i` committed prior to checkpoint
  - `<T_i commits>` appears before `<checkpoint>`
  - modifications made by `T_i` must have been written to stable storage prior to checkpoint or as part of it
- at recovery no need to redo such a transaction

### refining recovery algorithm
- search log backward for 1st checkpoint record
  - find transactions `T_i` following last checkpoint
  - `redo` & `undo` applied only to these transactions

## concurrent atomic transactions
- since each transaction is atomic
  - executed serially in some arbitrary order - serializability
  - maintained by executing each transaction with critical section - too restrictive
- allow transactions to overlap while maintaining serializability
  - concurrency control algorithms

### serializability
- serial schedule: each transaction executes atomically - n! schedules for n transactions

## locking protocols

### locking protocol governs how locks are acquired & released
- different modes in which data can be locked
  - shared mode locks (T~i~ can read but not write data item Q)
  - exclusive mode locks (T~i~ can read & write data item Q)

### transactions must request locks on data items in the right mode
- to access data item Q, T~i~ first must lock it
  - wait if Q is locked in exclusive mode
  - if T~i~ requests a shared lock on Q, obtain lock if Q not locked in exclusive mode
- T~i~ must hold lock on data item as long as it accesses it

### 2-phase locking protocol: locks & unlocks take place in 2 phases
- growing phase
  - obtain locks
  - cannot release any lock
- shrinking phase
  - can release locks
  - cannot obtain any new locks

### 2-phase locking protocol: conflict serializability
- conflicts occur when 2 transactions access same data item & one of them is a write
- a transaction acquires locks serially without releasing them during acquire phase
  - other transactions must wait for 1st transaction to start releasing locks
- deadlocks may occur

### order of conflicting transactions
- 2-phase locking determined at execution time
- timestamp-based protocols - selecting the order in advance

### timestamp-based protocols
- for each `T~i~` there is a fixed timestamp denoted `TS(T~i~)` assigned before `T~i~` starts execution
- for a later `T~j~`, `TS(T~i~) < TS(T~j~)`
- schedule must be equivalent to schedule in which `T~i~` appears before `T~j~`

### timestamp-based locking
- protocol ensures no deadlock - no transaction ever waits!
- conflict serializability
  - conflicting operations processed in timestamp order

### each data item Q has 2 values
- W-timestamp(Q)
  - largest timestamp of any transaction that successfully executed `write()`
- R-timestamp(Q)
  - largest timestamp of any transaction that successfully executed `read()`

### transaction issues `read(Q)`
- if TS(T~i~) < W-timestamp(Q)
  - needs value that was already overwritten
  - read rejected, T~i~ rolled back
- TS(T~i~) >= W-timestamp(Q)
  - operation executed
  - R-timestamp(Q) = max(TS(T~i~), R-timestamp(Q))

### transaction issues a `write(Q)`
- if TS(T~i~) < R-timestamp(Q)
  - value of Q produced by T~i~ needed previously
    - T~i~ assumed that this value would never be producec
  - `write` rejected, T~i~ rolled back
- if TS(T~i~) < W-timestamp(Q)
  - trying to write obsolete value of Q
  - `write` rejected, T~i~ rolled back

### what happens if transaction rolled back?
- transactions T~i~ assigned new timestamp -> restart

## deadlocks

### for many apps, procs need exclusive access to multiple resources
- proc a asks for scanner & is granted
- proc b asks for cd recorder & is granted
- proc a now asks for cd recorder
- proc b now asks for scanner
- both procs are blocked & will remain so forever -> deadlock

### other deadlock situations
- distributed systems involving multiple machines
- database systems
  - proc 1 locks record r1
  - proc 2 locks record r2
  - procs 1 & 2 try to lock each other's record -> deadlock
- deadlocks can occur in both hardware & software resources

### resource deadlocks
- major class of deadlocks involves resources
  - can occur when procs have been granted access to devices, data records, files, &c.
  - other classes of deadlocks: comm deadlocks, 2-phase locking
- related concepts
  - livelocks & starvation

### preemptable resources
- can be taken away from proc owning them with no ill effects
- e.g. memory
  - proc b's memory can be taken away & given to proc a
    - swap b from memory, write contents to backing store, swap a & let it use memory

### non-preemptable resources
- cannot be taken away from proc without causing it to fail
- if a proc has started to burn a cd, taking it away will fuck up the cd
- deadlocks usually involve non-preemptable resources

### notes on deadlocks
- os typically does not provide deadlock prevention facilities
- up to programmer to be responsible for designing deadlock-free programs

### system model
- finite number of resources distributed among competing procs
- resources partitioned into different types
  - each type has number of identical resources
  - e.g. memory space, files, i/o devices

### proc must utilize resources in sequence
- request
  - requesting proc must wait until it can acquire resource
  - `request()`, `open()`, `allocate()`
- use
  - operate on resource
- release
  - `release()`, `close()`, `free()`

### for kernel-managed resources, os maintains a system resource table
- is resource free? -> record proc that resource is allocated to
- is resource allocated? -> add to queue of procs waiting for resource
- for resources not managed by os, use `wait()` & `signal()` on semaphores

### deadlock: formal definition
- a set of procs is deadlocked if each proc in the set is waiting for an event that only another proc in the set can cause
- because all procs are waiting, none of them can cause events to wake any other member of set -> procs continue to wait forever

## deadlock characterization

### necessary conditions for deadlocks
- mutex
  - at least 1 resource held in nonsharable mode
  - when resource being used, another requesting proc must wait for its release
- hold-and-wait
  - a proc must hold 1 resource
  - wait to acquire additional resources which are currently held by other procs
- no preemption
  - resources cannot be preempted
  - only voluntary release by proc holding it
- circular wait
  - set of procs must exist that wait on each other
  - implies hold-and-wait

### resource allocation graph
- used to describe deadlocks precisely
- consists of set of vertices & edges
- 2 different sets of nodes
  - P: set of all active procs in a system
  - R: set of all resource types in a system

### directed edges 
- request edge (P~i~ -> R~j~)
  - P~i~ has requested an instance of resource type R~j~
  - directed edge from P~i~ to R~j~
  - currently waiting for that resource
- assignment edge (R~j~ -> P~i~)
  - instance of R~j~ assigned to P~i~
  - directed edge from R~j~ to P~i~

### determining deadlocks
- if graph contains no cycle, no proc in system deadlocked
- if graph contains cycle
  - if each resource has 1 instance, deadlock has occurred
  - if each resource has multiple instances, a deadlock may have occurred

### methods for handling deadlocks
- use protocol to prevent/avoid deadlocks
  - ensure system never enters deadlocked state
- allow system to enter deadlocked state, but detect & recover
- ignore problem, pretend deadlocks never occur

### problems w undetected deadlocks
- resources held by procs that cannot run
- more & more procs enter deadlocked state when they request resources
- deterioration in system performance -> requires restart

### when is ignoring problem viable?
- when they occur infrequently
  - ignoring is cheaper solution
  - prevention, avoidance, detection & recovery need to run constantly

## examples of deadlocks

### dining philosophers problem: necessary conditions for deadlock
- mutex
  - 2 philosophers cannot share same chopstick
- hold-and-wait
  - a philosopher picks up 1 chopstick at a time
  - will not let go of 1st while it waits for 2nd
- no preemption
  - philosopher doesn't snatch chopsticks held by some other philosopher
- circular wait
  - could happen if philosopher picks chopsick with same hand first

## dealing with deadlocks

### strats
- ignore problem
- detection & recovery
- deadlock avoidance
- deadlock prevention

### ostrich approach
- stick head in sand; pretend there is no problem
- reactions
  - mathematician: unacceptable, prevent at all costs
  - engineers: how often? costs?
- os suffer from deadlocks that are not even detected
  - num procs in system determined by slots in proc table (finite resource)
  - max num open files restricted by size of inode table
  - swap space on disk
  - every os table represents finite resource
  - should we abolish all tables because n procs would claim 1/n of total & try to claim another 1/n
  - most users prefer occasional deadlock to restrictive policy
    - e.g. all users: 1 proc, 1 open file, 1 everything far too restrictive
  - if deadlock elimination is free, then no discussions, but the price is often high due to inconvenient restrictions on procs -> tradeoff between convenience & correctness

## deadlock prevention

### deadlock prevention
- ensure that 1 of necessary conditions for deadlocks cannot occur
  - mutex
  - hold-and-wait
  - no preemption
  - circular wait

### mutex must hold for nonsharable resources, but...
- sharable resources do not require mutually exclusive access - cannot be involved in a deadlock
- a proc never needs to wait for a sharable resource (e.g. read-only files)
- some resources are intrinsically nonsharable, so denying mutex often not possible

### deadlock prevention: ensure hold-and-wait never occurs in system (strat 1)
- proc must request & be allocated all its resources before execution
  - resource requests must precede other system calls
  - e.g. copy data from dvd, sort file, & print
    - printer only needed at end, but proc will hold printer for entire execution

### deadlock prevention: ensure hold-and-wait never occurs in system (strat 2)
- allow proc to request resources only when it has none
  - release all resources before requesting additional ones
  - e.g. copy data from dvd, sort file, & print
    - first request dvd & disk file
      - copy & release resources
    - then request file & printer

### disadvantages of protocols doing hold-and-wait
- low resource utilization
  - resources allocated but unused for long durations
- starvation
  - if a proc needs several popular resources, popular resource might always be allocated to other procs

### deadlock prevention: eliminate preemption constraint
- case 1: if a proc is holding some resources
- case 2: proc requests another resource that cannot be immediately allocated
- all resources currently held by proc is preempted
  - preempted resoures added to list of resources proc is waiting for
- proc requests resources that are not currently available
  - if resources allocated to another waiting proc, preempt resources from 2nd, assign to 1st
- often applied when resource state can be saved & restored
  - e.g. cpu registers & memory space
  - unsuitable for tape drives

### deadlock prevention: eliminate circular wait
- impose total ordering of all resource types
  - assign each resource type a unique number
  - one-to-one function: F:R->N
    - F(tape drive) = 1
    - F(printer) = 12
1. request resources in increasing order
2. if several instances of a resource type needed, single request for all of them must be issued

### requesting resources in an increasing order of enumeration
- proc initially requested R~i~
- proc can now request R~j~ only if F(R~j~) > F(R~i~)
- alternatively, proc requesting R~j~ must have released resources R~i~ such that F(R~i~) >= F(R~j~)
- eliminates circular wait

### hierarchy of resources & deadlock prevention
- hierarchy by itself doesn't prevent deadlocks
  - developed programs must follow ordering
- F based on order of usage of resources
  - tape drive needed before printing: F(tape drive) < F(printer)

### deadlock prevention: summary
- prevent deadlocks by restraining how requests are made
  - ensure that at least 1 of 4 conditions cannot occur
- side effects:
  - low device utilization
  - reduced system throughput

### dining philosophers: deadlock prevention
- mutex
  - philosophers can share a chopstick
- hold-and-wait
  - philosopher should release 1st chopstick if it cannot obtain 2nd
- preemption
  - philosophers can forcibly take each other's chopstick
- circular wait
  - number chopsticks & pick up in ascending order

## deadlock avoidance

### deadlock avoidance
- require additional info about how resources are to be requested
- knowledge about sequence of requests & releases for procs
  - allows us to decide if resource allocation could cause a future deadlock
  - P: tape drive, then printer
  - Q: printer, then tape drive

### handling resource requests
- for each resource request, decide whether proc should wait to avoid future deadlock
- predicated on
  1. currently available resources
  2. currently allocated resources
  3. future requests & releases of each proc

### avoidance algorithms differ in amount & type of info needed
- resource allocation state
  - num available & allocated resources
  - max demands of procs
- dynamically examine resource allocation state
  - ensure circular wait cannot exist
- simplest model
  - declare max number of resources for each type
  - use info to avoid deadlock

### safe sequence
- sequence of procs for current allocation state
- resource requests made by P~i~ can by satisfied by
  - currently available resources
  - resources held by P~j~ where j < i
    - if needed resources not available, P~i~ can wait
  - in general, when P~i~ terminates, P~i+1~ can obtain its needed resources
- if no such sequence exists, system state is unsafe

### safe states
- conditions for a safe state
  - if system can allocate resources to each proc in some order up to max for proc while still avoiding deadlock

### safe states & deadlocks
- system is safe only if there is a safe sequence
- a safe state is not a deadlocked state
  - deadlocked state is unsafe state
  - not all unsafe states are deadlocks

### unsafe states
- unsafe state may lead to deadlock
- behavior of procs controls unsafe states
- cannot prevent procs from requesting resources such that deadlocks occur

### crux of deadlock avoidance algorithms
- ensure that system will always remain in safe state
- resource allocation request granted only if it will leave system in a safe state

## resource allocation graph algorithm

### claim edges
- indicates that P~i~ may request R~j~ at some time in future
- representation
  - same direction as request
  - dotted line

### conversion of claim edges
- when P~i~ requests resource R~j~, claim edge converted to request edge
- when R~j~ released by P~i~, assignment edge reconverted into claim edge

### allocating resources
- when P~i~ requests R~j~, request granted only if converting claim edge to assignment edge doesn't result in cycle

### note: resource allocation graph algorithm not applicable in systems with multiple resource instances

## banker's algorithm

### banker's algorithm
- designed by dijkstra in 1965
- modeled on small-town banker
  - customers have been extended lines of credit
  - not all customers will need max credit immediately
- customers make loan requests from time to time

### crux of banker's algorithm
- consider each request as it occurs
  - see if granting it is safe
- if safe, grant it; if unsafe, postpone
- for safety, banker checks if they have enough to satisfy some customer
  - if so, customer's loans are assumed to be repaid, customer closest to limit checked next
  - if all loans can be repaid, state is safe, loan approved
- declare max num resource instances needed
  - cannot exceed resource thresholds
- determine if resource allocations leave system in safe state

### data structures: n is num procs, m num resource types
- `available`: vector length m
  - num resources for each type
  - `available[i] = k`
- `max`: n * m matrix
  - max demand for each proc in ech row
  - `max[i][j] = k`
    - P~i~ may request at most k instances of R~j~
- `allocation`: n * m matrix
  - resource instances allocated for each proc (each row)
  - `allocation[i][j]` = k
    - P~i~ currently allocated k instances of R~j~
- `need`: n * m matrix
  - resource instances needed for each proc
  - `need[i][j] = k`
    - P~i~ may need k more instances of R~j~

### vectors identifying a proc's resource requirements: rows in matrices
- `allocation~i~` - resource instances allocated for P~i~
- `need~i~`: additional resource instances P~i~ may still request

### notations
- x, y vectors of length m
- x <= y <==> x[i] <= y[i] for all i
- e.g. x = { 1, 7, 3, 2 }, y = { 0, 3, 2, 1 } - y <= x, also y < x if y <= x & y != x

### resource request
- `request~i~`: request vector for P~i~
  - `request~i~[j] = k` - P~i~ wants k instances of R~j~

### limited practical value
- procs rarely know in advance about their max resource needs
- num procs not fixed, varies dynamically
- resources thought to be available can vanish
- few systems use this for avoiding deadlocks

## deadlock detection

### single instance of each resource type
- use wait-for graph - variant of resource allocation graph
- deadlock exists if there is cycle in graph
- transformation
  1. remove resource nodes
  2. collapse appropriate edges

### what edges in wait-for graph imply
- P~i~ -> P~j~ - P~i~ waiting for resource held by P~j~
- P~i~ -> P~j~ only if resource allocation graph has P~i~ -> R~q~ & R~q~ -> P~j~ for some R~q~

## deadlock detection

### deadlock detection for multiple instances of a resource type
- wait-for graph not applicable
- approach uses data structures similar to banker's algorithm

### data structures: n is num procs, m num resource types
- `available`: vector length m
  - num resources for each type
- `allocation`: n * m matrix
  - resource instances allocated for each proc
  - `allocation[i][j] = k` - P~i~ currently allocated k instances of R~j~
- `request`: n * m matrix
  - current request for each proc
  - `request[i][j] = k` - P~i~ requests k more instances of R~j~

### initialization
- `work` & `finish` are vectors length n
```c
work = available
if (allocation~i~ != 0) {
  finish[i] = FALSE;
} else {
  finish[i] = TRUE;
}
```

### usage
- how often will deadlock occur?
- how many procs will be affected when it happens?

### frequency of invoking deadlock detection
- resources allocated to deadlocked procs idle until deadlock can be broken
- deadlocks occur only when proc makes a request
  - significant overheads to run detection per request
- middle ground: run at regular intervals

## recovery from deadlock

### recovery from deadlock
- automated or manual
- options
  - break circular wait: abort procs
  - preempt resources from deadlocked procs

### breaking circular wait: proc termination
- abort all deadlocked procs
- abort procs one at a time
  - after each termination, check if deadlock persists
- reclaim all resources allocated to terminated procs

### terminating proc
- proc may be in midst of sth (e.g. updating files, printing data, &c.)
- abort proc whose termination will incur minimum costs
  - policy decision similar to scheduling decisions

### factors determining proc termination
- priority
- how long has proc been running & how much longer
- num & types of resources used & how many more needed
- interactive or batch

### resource preemption
- for a set of deadlocked procs
1. preempt resources from some proc
2. give resources to some other proc
3. if deadlock broken, done. else, goto 1
- issues
  - selecting victim
    - which resource & proc
    - order of preemption to minimize cost
  - starvation
    - proc can be selected for preemption a finite number of times

### deadlock recovery through rollbacks
- checkpoint progress periodically
  - contains memory image & resource state
- deadlock detection tells us which resources are needed
- proc owning a needed resource
  - rolled back to before it acquired needed resource
    - work done since rolled back checkpoint discarded
  - assign resource to deadlocked proc

## other issues

### 2-phase locking
- used in database systems
- operation involves requesting locks on several records & updating all locked records
- when multiple procs are running, possibility of deadlocks
- phase 1
  - proc tries to acquire all locks it needs, one at a time
  - if successful, start 2nd phase
  - if some record already locked, release all locks & restart phase 1
- phase 2
  - perform updates & release locks

### communication deadlocks
- proc a sends request message to proc b
  - blocks until b sends reply back
- suppose that request lost
  - a blocked waiting for reply, b blocked waiting for request to do sth -> communication deadlock
- cannot be prevented by ordering resources (none involved) or avoided by careful scheduling (no moments when requests can be postponed)
- solution: timeouts
  - start timer when you send a message to which a reply is expected

### livelocks
- polling (busy waits) used to enter critical section/access resource
  - typically used for a short time when overhead for suspension considered greater
- in a livelock 2 procs need each other's resource
  - both run & make no progress, but neither proc blocks
  - use cpu quantum over & over without making progress

### why livelocks occur
- if fork fails because proc table is full
  - solution: wait, try again
- but there could be a collection of some procs trying to do same thing

### starvation
- in dynamic systems, some policy is needed to make decision about who gets resource when
  - some procs never get service even though they are not deadlocked
- e.g. give printer to proc with smallest file to print
  - if there is constant stream of small jobs, proc with large file will starve
  - can be avoided with fcfs policy

