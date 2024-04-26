# fs

## toc

- fs structure
- fs implementation
- virtual fs
- allocations
  - contiguous
  - linked
  - indexed

## fs

### rationale

- apps need to store & retrieve info
  - programs can store limited amounts of info in their own addr spaces
  - storage capacity restricted to size of vram
    - far too small for several apps
      - airline reservations, banking, directory services, &c.
- info in addr space of procs is not persistent
  - when proc terminates, info is lost
  - for many apps, info must be retained for a much longer time - weeks, years, forever
- multiple procs often need to access parts of info at same time

### essential requirements for long-term storage

- store large amount of info
- info must survive proc termination
- multiple procs must be able to concurrently access info
- store info on disk/external media in units called files

### files are abstraction mechanism

- provide way to store info & read it back later
- do this in a way that shields user from knowing
  - how & where info stored on disk
  - how disks really work

### naming files

- important characteristic of abstraction mechanism
- strings 8-255 chars long
- most os support 2-part file names separated by period
  - last part referred to as file extension
    - conventions: easy to remember
    - enforced in some cases e.g. compiler

### files can be structured in many ways: unstructured sequence of bytes

- os doesn't know or care what is in file
  - max flexibility
- os doesn't help, but doesn't get in way
- meaning is imposed by programs
- most os support this

### file structure: sequence of records

- file is sequence of fixed-length records
- read operation returns 1 record
  - write operatin overwrites/appends 1 record
- 80-column punch card used to be dominant
  - files consisted of 80-char records

### file structure: tree of records

- get record with specific key
- os, not user, decides where to place new records

### dir & disk structure

- typically, there are millions of files within a computer
- storage device can be used in its entirety for a fs
- can also be partitioned
  - limit size of individual fs
  - put multiple fs tupes
  - set aside for swap space

### dirs used to organize files

- can be viewed as symbol table
- in many systems, dirs therefore are files
- supported operations
  - insert, delete, search, list, rename entries
  - traversal

### organization of dirs

- 1-level dir
- 2-level dir
- tree-structured dirs
- acyclic graph dirs
  - shared subdir

### mounting fs

- many systems have more than 1 disk - how to handle?
- solution 1: keep self-contained fs on each disk, keep them separate
- solution 2: allow a disk to be mounted in another disk's file tree

### checks performed during mounting

- os verified that device contains valid fs
- read device dir
  - make sure format is expected
- windows mounting
  - each device in separate namespace
  - letter followed by colon

## fs structure

### disks provide bulk of secondary storage

- disk can be rewritten in place
  - read, modify, then write back to same place
- disks can directly access any block of info
- i/o transfers between mem & disk performed in units of blocks

### 2 core design problems in fs

- defining how fs should look to user
- creating algorithms & data structures to map logical fs onto physical storage

### levels of fs (highest to lowest)

- apps
- logical fs
- file organization module
- basic fs
- i/o control
- devices

### i/o control consists of device drivers

- transfers info between main mem & disk
- receives high-level commands (e.g. retrieve block 123)
- outputs low-level, hardware-specific instructions
  - used by hardware controller
  - writes bit patterns into specific locations of i/o controller

### basic fs issues commands to device driver

- read & write physical blocks on disk (e.g. drive 1, cylinder 73, sector 10)
- manages buffers & caches
  - to hold fs, dir & data blocks
  - improves performance

### file organization module

- knows about files, both logical & physical blocks
- translate logical addrs to physical ones
  - needed for every block
- includes free space manager
  - tracks unallocated blocks & allocates as needed

### logical fs

- manages metadata info - data describing data
- manages file structure via file control blocks
  - info about file
    - ownership & permissions
    - location of file contents
  - inode in unix fs

### several fs are in use

- cd-roms written in iso 9660 format designed by cd manufacturers
- unix
  - unix file system (ufs)
  - bsd fast file system (ffs)
- windows
  - fat
  - fat32
  - ntfs
- linux
  - supports 40 different fs
  - extended fs: ext2, ext3, ext4

## anatomy of disk

### using magnet & nail for instant messaging

- message: "see you later, or not"
- drop nail in friend's mailbox
  - if nail magnetized, you'll see the friend
  - if nail not magnetized, you won't
- your friend comes home & picks up nail
  - use nail to pick up paperclip
    - if sticks, friend will expect to see you
- magnetism can be used to store info

### using magnetism to store info

- store info even when you turn power off
- storing 10001
  - magnetize 1st bit
  - demagnetize next 3
  - magnetize next bit

### anatomy of disk

- disk comprises set of platters
  - flat, circular shape
  - made of glass or aluminum
- both surfaces of platter covered with magnetic material
  - store info by recording it magnetically
- platter logically divided into circular tracks
  - subdivided into sectors

### rates & times associated with disks

- transfer rate: rate of data movement between disk & mem
- positioning time
  - seek time: time to move disk arm to necessary cylinder
  - rotational latency: time for desired sector to rotate to disk head

### how about cd-roms, dvds, & blu-rays?

- data written with help of high intensity laser that makes pits on reflecting surface
- when reading
  - use lower intensity laser
  - mirrors & focusing lens used to shine light on a specific portion of disk
  - amount of light that is reflected back depends on presence of pit (used to interpret 1 or 0)
- shorter wavelength, smaller pit, greater density of what can be stored
- dvd uses 650 nm wavelength laser diode
  - 780 nm for cd
  - pit sizes: dvd = 0.74 um, cd = 1.6 um
- blu-ray
  - 405 nm wavelength, 0.13 um pit size
  - 50 gb storage possible on 1 disk
- next
  - archival disk (sony/panasonic) 79.5 nm with 300gb of data storage

## fs implementation

### boot: etymology tidbit

- pull yourself up by your (own) bootstraps
- bring at least a portion of kernel into main mem, then have processor execute it

### on-disk structures used to implement fs

- boot control block
  - info needed to boot os from that volume
- volume control block: volume info
  - num blocks in partition
  - size of blocks
  - free block count/pointers
  - free file control block count/pointers
  - ufs: super block
  - windows: master file table
- dir structure to organize files (1 per fs)
- per-file file control block
  - contains details about individual files

### in mem structures used to improve performance via caching

- mount table
  - info about each mounted volume
- dir structure cache
  - holds info about recently accessed dirs
- system-wide open file table
  - file control block of each open file
- buffers to hold fs blocks
  - to read & write to storage

### creation of new file

- allocation of file control block (fcb)
- read appropriate dir into mem
  - dir is just a file in unix
    - special type field
- update dir with new file name & fcb
- write dir back to disk

### partitions: a disk can be sliced into multiple partitions

- cooked: has a fs
- raw: no fs
  - unix swap spaces use this
  - hold info needed by disk raid (redundant array of independent disks) systems

### boot info can be stored in separate partition

- usually sequential series of blocks loaded as image into mem
- image execution starts at predefined location

## dir implementation

### dir implementation

- linear list
- hash table

### linear list

- file names with pointers to data blocks
- simple to program, inefficient & slow execution
- finding file requires linear search
- sorted list
  - complicates creation & deletion
- tree data structures might help here
  - e.g. b-tree

### dir implementation: hash table

- linear list & hash table maintained
- key computed from file name
  - hash table value returns pointer to entry in linear list
- things to consider
  - account for collisions in hash space
  - need to rehash table when num entries exceed limit

### allocation methods: objective & approaches

- how to allocate space for files such that
  - disk space utilized effectively
  - file accessed quickly
- major methods
  - contiguous
  - linked
  - indexed

## contiguous allocations

### contiguous allocation

- each file occupies a set of contiguous blocks on disk
  - if file has n blocks & starts at location b, occupies blocks b, b + 1, ..., b + n - 1
- disk head movements
  - none for moving from block b to b + 1
  - only when moving to different track

### sequential & direct access in contiguous allocations

- sequential access
  - remember disk addr of last referenced block
  - when needed, read next block
- direct access to block i of file that starts at block b: b + i

### contiguous allocations suffer from external fragmentation

- free space broken up into chunks
  - space fragmented into holes
- largest continuous chunk may be insufficient for meeting request
- compaction very slow on large disks - needs several hours

### determining how much space needed for a file is another problem

- preallocate if eventual size of file is known
  - inefficient if file grows slowly
    - much of allocated space unused for long time
- modified contiguous allocation scheme
  - allocate space in a continuous chunk initially
  - when space runs out, allocate another set of chunks (extent)

## linked allocations

### linked allocation: each file is linked list of disk blocks

- each file block has a pointer to the next block
- file blocks, like with logical mem, do not need to be in physical order

### linked list allocations

- advantages
  - every disk block can be used
    - no space lost in external fragmentation
  - sufficient for directory entry to merely store disk addr of 1st block
    - the rest can be found starting there
- disadvantages
  - used effectively only for sequential accesses - extremely slow random access
  - space in each block set aside for pointers - each file requires slightly more space
  - reliability - what if pointer lost or damaged?

### linked list allocations: reading/writing files much less efficient

- amount of data storage in block is no longer a power of 2
  - pointer takes up some space
- peculiar size less efficient
  - programs read/write in blocks that are powers of 2
