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

