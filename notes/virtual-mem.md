# virt mem

## toc

- virt mem
- demand paging
- performance of demand paging
- page replacement

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
