# scheduling algorithms

## scheduling algorithms
- decides which proc in ready queue allocated cpu
- could be preemptive or nonpreemptive
- optimize measure of interest
- use Gantt charts to illustrate schedules
  - bar chart with start & finish times for procs

### first-come first-served (fcfs)
- first proc requesting cpu gets it first
- managed with fifo queue
  - when proc enters ready queue, pcb tacked to tail of queue
  - when cpu is free, allocated to proc at head of queue
- simple to write & understand
- avg waiting time - take average of all procs scheduled
  - varies greatly based on which proc starts when

### shortest job first (sjf)
- when cpu available, assigned to proc with smallest cpu burst
- moving a short proc before a long proc -> reduction in waiting time for short proc greater than increase in waiting time for long proc
- gives us minimum avg waiting time for a set of procs that arrived simultaneously - provably optimal
- optimal only if all jobs are available simultaneously
  - consider 5 procs a (2), b (4), c (1), d (1), e (1)
    - arrival times are 0, 0, 3, 3, 3
  - sjf will run jobs a, b, c, d, e
    - avg waiting time: 2.8
    - but if you run b, c, d, e, and a, avg wait time = 2.6
- preemptive sjf
  - a new proc arrives in ready queue
    - if it is shorter than currently existing proc, we preempt the current proc
- long-term schedulers
  - length of proc time limit used as cpu burst estimate
  - motivate users to accurately estimate time limit
    - lower value -> faster response times
    - too low a value -> time limit error - requires resubmission
- short-term schedulers
  - no way to know length of next cpu burst
  - try to predict length
  - procs scheduled based on predicted cpu bursts

### prediction of cpu bursts
- `t_n`: length of nth cpu burst
- `tau_n`: estimate for nth cpu burst
- `alpha`: controls weight of recent & past history
- `tau_(n + 1) = alpha * t_n + (1 - alpha) * tau_n`
- `alpha` contians relative weight of recent & past history
  - value of `t_n` contains our most recent information, while `tau_n` stores the past history
  - `tau_(n + 1) = sum(n = 0, n, alpha * t_n + (1 - alpha) * tau_n)`
  - `alpha < 1`

## priority scheduling

### priority scheduling
- priority associated with each proc
- cpu allocated to proc with highest priority
- can be preemptive or nonpreemptive
  - if preemptive: preempt cpu from lower priority proc when higher priority one is ready

### how priorities are set
- internally defined priorities depend on measured quantities
  - time limits, mem requirements, # open files, ratio (avgs) of i/o to cpu burst
- external priorities
  - criteria outside purview of os
  - importance of proc, $ paid for usage, politics, &c.

### issues with priority scheduling
- can leave lower priority procs waiting indefinitely
- e.g. mit's ibm 7094 shutdown (1973) found procs from 1967

### coping with issues in priority scheduling: aging
- gradually increase priority of procs that wait for a long time
- e.g. proc with priority 127, increases every 15 minutes - priority becomes 0 in no more than 32 hours

## lottery scheduling

### lottery scheduling
- give procs lottery tickets for various system resources e.g. cpu time
- when scheduling decision has to be made, lottery ticket chosen at random, proc holding ticket gets resource
- properties
  - highly responsive - chance of winning proportional to tickets
  - cooperating procs may exchange tickets
    - e.g. proc a sends request to b, hands b all its tickets for a faster response
  - avoids starvation
    - each proc has > 1 ticket - guaranteed to have a non-0 probability of being scheduled
  - solves problems that are difficult to handle in other scheduling algorithms
    - e.g. video server managing procs that feed video frames to clients
      - clients need frames at x, y, z fps
      - allocate procs x, y, z tickets - cpu divided approx 10:20:25

### all procs are equal, but some are more equal than others
- more important procs given extra tickets to increase their odds of winning
- a proc holding a fraction of tickets will get about a fraction of the resource

## round-robin scheduling

### round-robin scheduling
- similar to fcfs - preemption to enable switch between procs
- ready queue implemented as fifo
  - proc enters pcb at tail of queue
  - proc chosen from of queue
- scheduler goas around ready queue
  - allocates cpu to eac proc one after the other
    - cpu-bound up to a maximum of 1 quantum

### choosing quantum
- context switch time is consuming
  - saving & loading registers & memory maps
  - updating tables
  - flushing & reloading memory cache
- what if quantum is 4ms & context sitch overhead is 1ms?
  - 20% of cpu time thrown away in administrative overhead

### improve efficiency by increasing quantum
- quantum: 100ms, context-switch overhead: 1ms (wasted time: 1%)
- what if 50 concurrent requests come in?
  - each with widely varying cpu requirements
  - 1st starts immediately, 2nd 100ms later, ...
  - last one may have to wait for 5 seconds!
  - a shorter quantum would have given them better service

### if quantum set longer than mean cpu burst
- preemption will not happen very often
- most procs will perform a blocking operation before quantum runs out
- switches only happen when proc blocks & cannot continue

## multi{processor,core} environments

### load balancing: migration based approaches
- push migration
  - specific task periodically checks for imbalance
  - balances load by pushing procs from overloaded to less-busy processors
- pull migration
  - idle processor pulls waiting task from busy processor
- schemes not mutually exclusive: used in parallel
  - linux runs a load-balancing algorithm 
    - every 200ms - push migration
    - when processor run queue empty - pull migration

### multicore processors place multiple processor cores on same processor chip
- each core has its own register set - appears to os as separate physical processor
- modern designs implement 2+ hardware threads per core
  - if there is a mem stall (due to cache miss) on one thread, switch to another hardware thread

### multithreading a processor
- coarse grained
  - thread executes on processor until memory stall
  - switch to another thread
- switching between threads
  - flush instruction pipeline
  - refill pipeline as new thread executes
- finer grained (interleaved)
  - switch between threads at boundary of instruction cycle
  - design includes logic for thread switching - overheads low

### tiered scheduling on multicore processors
- first level: os
  - os chooses which hardware thread to run on each hardware thread
- second level: core
  - decides which hardware thread to run

## linux scheduling

### highlights of linux scheduling
- scheduling algorithm runs in constant time
- implements real-time scheduling (posix 1.b)
  - real-time tasks have static priorities
  - other tasks have dynamic priorities
- 2.5 algorithm
  - revised again in 2.6.23 (completely fair scheduler)
- preemptive, priority-based
- 2 separate priority ranges
  - real-time range: 0-99
  - nice value: 100-140
- numerically lower values indicate higher priority
- unlike solaris & windows
  - higher priority tasks = higher quanta
  - lower priority tasks = lower quanta
- task's interactivity determined by sleeping times for i/o

### task execution
- task is eligible for execution as long as it has time

