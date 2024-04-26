# virt mem

## toc

- virt mem
- demand paging
- performance of demand paging
- page replacement
- page replacement algorithms
- page buffering
- frame allocations
- working sets
- tlb reach

### how we got here

- contiguous mem -> external fragmentation -> pure paging -> low degree of multiprogramming -> virt mem

## virt mem

### logical view of proc in mem

- stack: function params, return addrs, local vars
- in between: requires actual physical space only if heap or stack grows
- heap: mem allocated dynamically during runtime
- data: global vars
- text: program code

### sparse addr spaces

- virt addr spaces with holes
- harnessed by
  - heap or stack segments
  - dynamically linked libs

## demand paging

### loading an executable program into mem

- what if we load entire program?
  - may not need entire program
- load pages only when needed
  - demand paging

### differences between swapper & pager

- swapper
  - swaps entire program into mem
- pager
  - lazy swapper
  - never swap page into mem unless actually needed

#### swapping: temporarily moving proc out of mem into backing store

#### paging: swapping pages in & out of physical mem

### demand paging: basic concepts

- guess pages to be utilized by proc before proc will be swapped out
- avoid reading unused pages
  - better physical mem utilization
  - reduced i/o -> lower swap times

### distinguishing between pages in mem & those on disk

- valid-invalid bits
  - associated with entries in page table
- valid
  - page both legal & in mem
- invalid - one or other
  - page not in logical addr space of proc
  - valid but currently on disk

#### page table keeps track of which pages are valid & which are invalid

### handling valid-invalid entries in page table

- if proc never attempts to access invalid page, no problem
- if proc accesses page not mem resident, page fault

### handling page faults

1. load M, which makes a reference to page table
2. if invalid, trap to os
3. from os, locate page on backing store
4. bring in missing page from backing store & put it in free frame
5. reset page table & change entry validity
6. restart instruction

### pure demand paging

- never bring page into mem unless required
- execute proc with no pages in mem
  - 1st instruction of proc will fault for page
- page fault to load page into mem & execute
- problems
  - multiple page faults per instruction execution
    - 1 fault for instruction
    - many faults for data
  - multiple page faults per instruction are rare - locality of reference

### hardware requirements to support demand paging

- page table
- secondary mem - swap space

### restarting instructions after page fault

- page faults occur at mem reference
- use pcb to save state of interrupted proc
- restart proc exactly in same place
  - desired page now in mem & accessible

### restarting procs after page fault serviced

- if fault occurred during instruction fetch, during restart, refetch instruction
- if fault occurred while fetching operands
  1. fetch & decode instruction
  2. fetch operand

### worst case example

- add operands a & b, place sum in c
- if fault while storing c
  - service page fault
  - update page table
  - restart instruction
    - decode, fetch operands, perform addition

### problems when operations modify several different mem locations

- e.g. move block from 1 mem location to another
- case 1: either block straddles page boundary
- case 2: page fault occurs
- move might be partially done, which is a big problem

### approaches to fault-proofing block transfers

1. compute & access both ends of block

- if page fault were to happen, it will at this point
  - nothing partially modified
- after fault servicing, block transfer completes

2. use temp registers

- track overwritten values

### can on-demand paging be applied anywhere without modifications?

- paging is between cpu & physical mem - transparent to user procs
- non-demand paging can be applied to any system
- demand paging cannot be applied to any system
  - fault processing of special instructions not trivial

## performance of demand paging

### effective access times

- without page fault, effective access times are equal to mem access times
  - ~200 ns
- with page faults, account for fault servicing with disk i/o

### calculating effective access times with demand paging

- p: prob of page fault
- ma: mem access time
- pft: page fault time
- effective access time = (1 - p) _ ma + p _ pft

### components of page fault servicing

- service interrupt: 1-100 us
- read in page: latency 3 ms, seek 5 ms
- restart proc: 1-100 us

### effective access times

- effective access time = (1 - p) _ ma + p _ pft = 200 ns + 7999800 \* p

### if performance degradation is to be less than 10%

- 220 > 200 + 7999800 \* p
  - p < 0.0000025

## other issues in demand paging

### allocation of physical mem to i/o & programs is a challenge

- mem used for holding program pages
- i/o buffers also consume big chunk of mem
- solutions
  - fixed percentage for i/o
  - procs & i/o subsystem compete

### demand paging & limits of logical mem

- without demand paging
  - all pages of proc must be in physical mem
  - logical mem limited to size of physical mem
- with demand paging
  - all pages of proc need not be in physical mem
  - size of logical addr space no longer constrained by physical mem

### demand paging is os's attempt to improve cpu utilization & system throughput

- load pages into mem when referenced
  - increases degree of multiprogramming
- e.g.
  - 40 pages of physical mem
  - 6 procs, each of which is 10 pages
    - each proc only needs 5 pages rn
  - run 6 procs with 10 pages to spare

### increasing degree of multiprogramming can be tricky

- essentialy we are over-allocating physical mem
- e.g.
  - physical mem: 40 pages
  - 6 procs, each of which is size 10 pages
    - each proc only needs 5 pages rn
  - what happens if each proc needs all 10 pages?
    - 60 physical frames needed

### coping with over-allocating mem

- terminate user proc, but paging should be transparent to user
- swap out proc - reduces degree of multiprogramming
- page replacement

### 2 core problems in demand paging

- frame allocation
  - how many frames to allocate to proc
- page allocation
  - select frame(s) for replacement
- caveat
  - disk i/o expensive so inefficient solutions can weigh things down

## page replacement

### page replacement

- if no frame is free, find one that is not currently being used & use it

### freeing physical mem frame

- write frame contents to swap space
- change page table of proc to reflect that it's no longer in mem
- freed frame can now hold some other page

### servicing page fault

1. retrieve page from disk
2. is free frame available?

- yes: use it
- no: use page replacement algorithm to select victim frame, write victim frame to disk, then use frame

#### page replacement central to demand paging

### overheads for page replacement

- if no frames free, 2 page transfers needed
  - victim page out
  - new page in
- if no free frames
  - doubles page fault service time
  - increases effective access time

### using modify bit to reduce page replacement overheads

- each page/frame has modify bit
  - set by hardware when page is written
  - indicates if page modified since last time read from disk
- during page replacement
  - if victim page not modified, no need to write it to disk - reduces i/o time by 1/2

## page replacement algorithms

### page replacement algorithms

- goal: low page fault rates
- how to evacuate?
  - run algorithm on string of mem references - reference string
  - compute num page faults

### reference string: snapshot mem references

- we track page nums, not entire addr
- if we have a reference to mem-resident page p
  - any references to p that follow will not page fault since page already in mem mem

### factors involved in determining page faults

- reference string of executing procs
- page replacement algorithm
- num physical mem frames available
- intuitively
  - page faults reduce as num page frames increase

### fifo page replacement algorithm

- when page must be replaced, replace oldest one
- os maintains list of all pages currently in mem
  - page at head of list: oldest one
  - page at tail: newest arrival
- during page fault
  - page at head removed
  - new page added to tail

## optimal page replacement algorithm

### optimal page replacement algorithm

- best possible algorithm
- easy to describe but impossible to implement
- crux: put off unpleasant stuff for as long as possible

### optimal page replacement algorithm description

- when page fault occurs some set of pages are in mem
- one page will be referenced next
  - other pages may not be referenced until 10, 100, or 1000 instructions later
- label each page with num instructions to be executed before it will be referenced
  - when there is a page fault, page with highest label should be removed

### problem with optimal page replacement algorithm

- unrealizable
- during a page fault, os has no way of knowing when each page will be referenced next

### why look at it?

- run a program
  - track all page references
- implement optimal page replacement on 2nd run based on reference info from first run
- compare performance of realizable algorithms with best possible one

## lru page replacements

### least recently used (lru) page replacement algorithm

- approximation of optimal algorithm
- observation
  - pages used heavily in last few instructions probably will be used heavily in next few
  - pages that have not been used will probably remain unused for a long time
- when page fault occurs, throw out page that has been unused for longest

### implementing lru

- logical clock
- stacks

### using logical clock to implement lru

- each page table entry has time-of-use field
  - entry updated when page referenced
    - contents of clock register copied
- replace page with smallest value
  - time increases monotonously
    - overflows must be accounted for
- requires search of page table to find lru page

### stack based approach

- keep stack of page nums
- when page referenced, move to top of stack
- implemented as doubly linked list
- no search done for replacement
  - bottom of stack is lru page

### problems with clock/stack based approaches to lru replacements

- inconceivable without hardware support
  - few systems provide requisite support for true lru implementations
- updates of clock fields or stack needed at every mem reference
- if we use interrupts & do software updates of data structures, things would be very slow (every mem reference >10x slower)

## lru approximation page replacements

### lru approximation: reference bit

- reference bit associated with page table entries
- reference bit set by hardware when page referenced
  - r/w access of page
- determine which page has been used, which has not
  - however, no way of knowing order of references

### lru approximation: additional reference bits

- maintain byte for each page in mem
- os shifts reference bit for page into highest order bit of byte
  - operation performed at regular intervals
  - reference bit then cleared

### lru approximation: interpreting reference bits

- interpret bytes as unsigned ints
- page with lowest num is lru page
- 00000000: not used in last 8 periods
- 01100101: used 4 times in last 8 periods
- 11000100 used more recently than 01110111

### 2nd chance algorithm

- simple modification of fifo
- avoids throwing out heavily used page
- inspect reference bit of page
  - if 0: page old & unused -> evict
  - if 1: page given 2nd chance -> move page to end of list
- reasonable, but unnecessarily inefficient
  - constantly moving pages around on its list
- better to keep pages in circular list like a clock

### clock page replacement

- keep all frames on circular list like a clock
  - hand points to oldest page
- when page fault occurs, page being pointed to by hand inspected
  - if r bit 0, page evicted
    - new page inserted into clock in its place
    - hand advances 1 position
  - if r bit 1, cleared
    - hand advances 1 position

### counting based page replacements: most frequently used (mfu)

- argument: page with smallest count was probably just brought in

### summary of page replacement algorithms

| algorithm                       | comment                                        |
| ------------------------------- | ---------------------------------------------- |
| optimal                         | not implementable, but useful as benchmark     |
| fifo                            | might throw out important pages                |
| 2nd chance                      | big improvement over fifo                      |
| clock                           | realistic                                      |
| lru (least recently used)       | excellent, but difficult to implement          |
| nfu (not frequently used)       | fairly crude approximate to lru                |
| aging (multiple reference bits) | efficient algorithm that approximates lru well |

## page buffering algorithms

### page buffering

- maintain buffer of free frames
- when page fault occurs
  - victim frame chosen as before
  - desired page read into free frame from buffer before victim frame written out
  - proc that page faulted can restart much faster

### page buffering: being proactive

- maintain a list of modified pages
- when paging device idle, write modified pages to disk
- implications
  - if page selected for replacement, increase likelihood of that page being clean

### page buffering: reuse what you can

- keep pool of free frames as before, but remember which pages they held
- frame contents not modified when page written to disk
- if page needs to come back in, reuse same frame if it was not used to hold some other page

### buffering & apps

- apps often understand their mem/disk usage better than os
  - provide their own buffering schemes
- if both os & app were to buffer, 2x i/o being utilized for a given i/o

## allocation of frames

### constraints on frame allocation

- max: total num frames in system
  - available physical mem
- min: need to allocate at least a min num frames defined by system architecture

### min num frames

- as you decrease num frames for a proc, page fault & execution time increase
- defined by architecture
  - in some cases, instructions & operands (indirect references) straddle page boundaries
    - with 2 operands, >6 frames needed

## frame allocation policies

### global vs. local allocation

- global replacement
  - 1 proc can take a mem frame from another proc
- local replacement
  - proc can only choose from set of frames allocated to it

## working sets & thrashing

### locality of references

- during any phase of execution, a proc references a relatively small fraction of its pages
- working set - set of pages that a proc is currently using

### implications of working set

- if entire working set in mem, proc will execute without causing many faults until it moves to another phase of execution
- if available mem too small to hold working set
  - proc will cause many faults
  - run very slowly

### program causing page faults every few instructions is said to be thrashing

- system throughput plunges
  - procs spend all their time paging
- increasing degree of multiprogramming can cause this
  - new proc may steal frames from another proc
    - overall page faults in system increases

### mitigating effects of thrashing

- using local page replacement algorithm
  - 1 proc thrashing doesn't cause cascading thrashing for other procs
  - but if a proc is thrashing, average service time for page fault increases
- best approach
  - track proc's working set
  - make sure working set in mem before you let it run

## working sets & thrashing

### working set is approximation of program's locality

- most important property of working set: size
- WSS_i: working set size for proc p_i
- if total demand exceeds available frames, thrashing will occur

### working sets & page fault rates

- peak in page fault rate happens when new locality being demand-paged
- once working set in mem, page fault rate fails
- when proc moves towards a new working set window, fault rate rises again

### page fault frequency approach to reducing thrashing

- when page fault rate high, proc needs more frames
- when page fault rate too low, proc may have too many frames

## other considerations

### prepaging: loading pages before letting proc run

- at 1 time, bring into mem all pages that will be needed
  - prepage frames for small files
- with working set model, ensure that entire working set in mem before proc resumed

### tlb reach is amt mem accessible from tlb

- tlb reach = num tlb entries \* page size
- approaches to increasing tlb reach
  - double entries (expensive)
  - increase page size (increases internal fragmentation)
  - support multiple page sizes (current trend)
    - os manages tlb
    - increase reach & hit ratio

### select data structures & program structures efficiently

- increase locality
  - reduce page fault rates
- loops
  - if data stored in row major format, but program needs it as column major format
- loader shoudl avoid placing routines across page boundaries
