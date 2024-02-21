# inter-proc communications

## shell
- forks children to execute commands
- waits for children to finish
- when stdio comes from terminal, terminate command w interrupt character <C-c>

## posix (portable operating systems interface for unix)
- 2 distinct, incompatible flavours of unix existed (sysv, bsd)
- programs written for one type of unix didn't run correctly/compile on unix from different vendor

### ieee attempt to develop standard for unix libs
- posix.1 published in 1988 - covered small subset of unix
- 1994 - x/open foundation had much more comprehensive effort called spec 1170 based on sysv
- inconsistencies between posix.1 & spec 1170
- 1998
    - another version of x/open standard
    - many additions to posix.1
    - austin group formed
- result: no need to contend w small but critical variations in library fxns across platforms

## inter-proc comms

### independent & cooperating procs
- independent: cannot affect/be affected by other procs
- dependent: can affect/be affected by other procs

### why have cooperating procs?
- info sharing: shared files
- computational speedup - subtasks for concurrency
- modularity
- convenience: do multiple things in parallel
- privilege separation
- ...

### cooperating procs need ipc to exchange data & info
- shared memory
    - establish memory region to be shared
    - read/write to shared region

### using shared mem
- os typically prevents processes from accessing others' memories - workaround
    1. both agree to remove this restriction 
    2. coordinate access to this region

### circular buffer: bounded
- in: next free position (producer)
- out: first full position (consumer)
- after producing: `in = (in + 1) % BUFFER_SIZE`
- after consuming: `out = (out + 1) % BUFFER_SIZE`
- if `(in + 1) % BUFFER_SIZE == out`, then buffer is full

### creating a memory segment to share
- create shared memory segment `shmget(IPC_PRIVATE, size, S_IRUSR | S_IWUSR)`
    - `IPC_PRIVATE`: key for segment
    - `size`: size of shared memory
    - `S_IRUSR|S_IWUSR`: mode of access (read, write)
- successful invocation of `shmget()`
    - return integer ID of shared segment
        - needed by other procs that want to use region

### attach memory segment
- `shmat()` - return pointer to beginning location in memory
- `(char  *) shmat (id, asmP, mode)`
    - `id`: int id of memory segment being attached
    - `asmP`: pointer location to attach shared memory
        - `NULL` allows os to select location for you
    - `mode` indicates read/write
        - 0 reads & writes to shared memory

### what happens when done?
- detach using `shmdt(shared_memory)`
- to remove a shared memory segment - `shmctl(id, IPC_RMID, shmR)`

### communicate & sync actions wout sharing address space
- useful in distributed environments (e.g. message passing interface)
- two main operations
    - `send(message)`
    - `receive(message)`
- message sizes can be
    - fixed: easy
    - variable: little more effort

### comms between procs
- must be physical link
- underlying physical implementation
    - shared memory
    - hardware bus
    - network

### aspects to consider for ipc
1. comms
    - direct/indirect
2. synchronization
    - sync/async
3. buffering
    - automatic/explicit buffering

### direct comms
- explicitly name recipient/sender
- link automatically established - exactly 1 between 2 processes
- addressing
    - symmetric
    - asymmetric

### direct comms - addressing
- symmetric addressing - explicitly name recipient & sender
    - `send(P, message)`
    - `receive(Q, message)`
- asymmetric addressing - only sender names recipient
    - `send(P, message)`
    - `receive(id, message)`
        - id set to name of sending proc

### direct comms - disadvantages
- limited modularity
- cascading effects of changing identifier of proc
    - examine all other proc definitions

### indirect comms
- each mailbox (port) has unique id & owner
    - posix message queues use ints to id ports
- procs communicate only if they have shared port
    - `send(A, message)`
    - `receive(A, message)`
- link properties
    - link established only if both members share port
    - link may be associated w more than two procs
- procs P1, P2, P3 share mailbox A
    - P1 sends message to A
    - P2, P3 execute `receive()` from A
- possibilities allow for
    1. link to be associated w at most 2 procs
    2. at most 1 proc to execute `receive()` at a time
    3. system to arbitrarily select who gets message

### port ownership: owned by os
- port has its own existence
- port is independent - not attached to any procs
- os must allow procs to
    - create port
    - send/receive through port
    - delete port

### message passing: synchronization issues - options for implementing primitives
- blocking send
    - block until received by proc/port
- nonblocking send
    - send & promptly resume other operations
- blocking receive
    - block until message available
- nonblocking receive
    - retrieve valid message or null
- producer/consumer problem: easy w blocking

### buffering
- messages exchanged by communicating procs reside in a temporary queue
- implementation schemes for queues
    - zero capacity - no messages can reside in queue
    - bounded - some messages can reside in queue
    - unbounded - queue adapts for amount of messages

