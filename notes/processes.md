# Processes

### Process
- Unit of work in most systems
- Just an instance of an executing program
- Each process has its own virtual CPU
- Processes not affected by multiprogramming
- At any instant only one process executes

### When a process is interrupted
- Computer must appreciate urgency

### How a program becomes a process
- When program is run, OS copies program image into main memory
- Allocation of memory not enough to make program into process
- Must have process ID
- OS tracks IDs & process states to orchestrate system resources

### Program in memory
- Program image appears to occupy contiguous blocks of memory
- OS maps programs into non-contiguous blocks
- Mapping divides program into equal-sized pieces called pages
- OS loads pages into memory
- When processor references memory on page, OS looks up page in table & loads it into memory

### Advantages of mapping
- Allows large logical address space for stack & heap - no physical memory used unless actually needed

### Process control block (PCB)
- How processes are represented
- Contains
    - State
    - Number
    - Counter
    - Registers
    - Memory limits
    - List of open files
- Repository of process information

### Scheduling queues
- Job queue: contains all processes
    - Newly created processes enter here first
- Ready queue
    - Processes remaining in main memory
    - Ready & waiting to execute
    - Typically linked list
- Device queue
    - Processes waiting for particular I/O device

### Process migration
- Long-term scheduler: batch systems
    - Executes much less frequently
    - Can take more time to decide what to select
- Short-term scheduler
    - Frequently processes for CPU
    - Selected process executes every few ms
    - Typically, execute every 10-100ms
- Unix & Windows often have no long-term scheduler
    - Every new process goes on short-term
    - Stability depends on physical limitations & self-adjusting nature of users

## Interrupts & contexts

### Factors that impact speed of context switch
- Memory speed
- Number of registers to copy
- Special instructions for loading/storing registers
- Memory management: preservation of address space

## How to create subprocesses
- Use `fork()`
    - When `fork()` fails
        - No child is created
        - `fork()` returns -1

